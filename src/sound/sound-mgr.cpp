#include <OpenAL-soft/AL/al.h>
#include <OpenAL-soft/AL/alc.h>
#include <atomic>
#include <thread>
#include <cstdint>
#include <cstring>
#include <cassert>
#include <functional>
#include <utility>
#include <unordered_map>
#include "sound-mgr.hpp"
#include "packet-decoder.hpp"
#include "util/log.hpp"
#include "util/error.hpp"
#include "util/str-util.hpp"

inline Str decode_oal_error(const ALenum error_enum) {
  switch (error_enum) {
    default:
    case AL_NO_ERROR: return "OAL no errors"; break;
    case AL_INVALID_NAME: return "OAL invalid name"; break;
    case AL_INVALID_ENUM: return "OAL invalid enum"; break;
    case AL_INVALID_VALUE: return "OAL invalid value"; break;
    case AL_INVALID_OPERATION: return "OAL invalid operation"; break;
    case AL_OUT_OF_MEMORY: return "OAL out of memory"; break;
  }
  return "unknown OAL error";
}

inline void check_oal_error(cr<Str> func_name) {
  if (cauto error = alGetError(); error != AL_NO_ERROR)
    error("error in OAL function: " + func_name + " - " + decode_oal_error(error));
}

struct Sound_mgr_oal::Impl {
  Sound_mgr_config m_config {};

  struct Audio_info {
    Vector<ALuint> oal_buffer_ids {};
    ALuint oal_source_id {};
    Shared<Packet_decoder> packet_decoder {}; // выдаёт поблочно данные аудио файла
    cp<Audio> sound {}; // прикреплённый звук из банка
    Audio_id audio_id {BAD_AUDIO}; // ID для управления звуком

    inline void update(Impl* master) {
      assert(packet_decoder);
      assert(sound);
      assert(oal_buffer_ids.size() >= master->m_config.buffers);

      // узнать состояние буфферов
      ALint buffers_processed = 0;
      alGetSourcei(oal_source_id, AL_BUFFERS_PROCESSED, &buffers_processed);
      check_oal_error("alGetSourcei AL_BUFFERS_PROCESSED");
      crauto format = master->to_compatible_oal_format(*sound);

      while (buffers_processed > 0) {
        ALuint buffer_id;
        alSourceUnqueueBuffers(oal_source_id, 1, &buffer_id);
        check_oal_error("unqueue buffer");

        cauto decoded_buffer = packet_decoder->decode(master->m_config.buffer_sz);
        cauto sound_buffer = format.converter(decoded_buffer,
          decoded_buffer.size() / format.bytes_per_sample);
        alBufferData(buffer_id, format.oal_format, sound_buffer.data(),
          sound_buffer.size(), sound->frequency);
        check_oal_error("alBufferData");
        
        alSourceQueueBuffers(oal_source_id, 1, &buffer_id);
        check_oal_error("re-enqueueing buffer to source");
        
        --buffers_processed;
      }
    } // update
  }; // Audio_info

  Audio_id m_uid {}; // для генерации audio_id
  bool m_eax_2_0_compat {}; // совместимость с эффектами EAX 2.0
  std::unordered_map<Str, Audio> m_store {}; // хранит исходники звуков с привязкой по имени
  std::unordered_map<Audio_id, Audio_info> m_audio_infos {}; // контролирует статус воспроизведения
  std::thread m_update_thread {}; // поток для обновления состояния и пакетного декода
  std::atomic_bool m_update_thread_live {}; // false - завершить m_update_thread
  mutable std::mutex m_mutex {}; // блокирует доступ к внутренностям

  inline explicit Impl(cr<Sound_mgr_config> config): m_config {config} {
    assert(config.buffer_sz >= 1024 * 8);
    assert(config.buffers > 2);
    assert(config.sounds >= 8);
    init_openal();
    make_update_thread();
  }

  inline ~Impl() {
    m_update_thread_live = false;
    if (m_update_thread.joinable())
      m_update_thread.join();
    close_oal();
  }

  inline Audio_id play(cr<Str> sound_name, const Vec3 source_position,
  const Vec3 source_velocity, const real amplify, const bool repeat) {
    // звуки с нулевой громкостью не запускать
    return_if (amplify <= 0, BAD_AUDIO);

    if (m_audio_infos.size() >= m_config.sounds) {
      log_debug << "no more OAL buffer/source";
      return BAD_AUDIO;
    }

    crauto sound = find_audio(sound_name);
    Audio_info audio_info;
    audio_info.packet_decoder = make_packet_decoder(sound);
    audio_info.sound = &sound;

    std::lock_guard lock(m_mutex);
    alGenSources(1, &audio_info.oal_source_id);
    check_oal_error("alGenSources");
    audio_info.oal_buffer_ids.resize(m_config.buffers);
    alGenBuffers(m_config.buffers, audio_info.oal_buffer_ids.data());
    check_oal_error("alGenBuffers");

    // перенести семплы в буферы
    crauto format = to_compatible_oal_format(*audio_info.sound);
    cfor (buffer_id, m_config.buffers) {
      cauto decoded_buffer = audio_info.packet_decoder->decode(m_config.buffer_sz);
      cauto sound_buffer = format.converter(decoded_buffer,
        decoded_buffer.size() / format.bytes_per_sample);
      alBufferData(audio_info.oal_buffer_ids.at(buffer_id), format.oal_format,
        sound_buffer.data(), sound_buffer.size(), audio_info.sound->frequency);
      check_oal_error("alBufferData");
    }
    // привязать буферы к источнику
    alSourceQueueBuffers(audio_info.oal_source_id, m_config.buffers,
      audio_info.oal_buffer_ids.data());
    check_oal_error("alSourceQueueBuffers");

    // настроить свойства звука
    alSourcef(audio_info.oal_source_id, AL_GAIN, amplify);
    check_oal_error("alSourcef AL_GAIN");
    alSource3f(audio_info.oal_source_id, AL_POSITION,
      source_position.x, source_position.y, source_position.z);
    check_oal_error("alSource3f AL_POSITION");
    alSource3f(audio_info.oal_source_id, AL_VELOCITY,
      source_velocity.x, source_velocity.y, source_velocity.z);
    check_oal_error("alSource3f AL_VELOCITY");
    alSourcei(audio_info.oal_source_id, AL_LOOPING, repeat ? AL_TRUE : AL_FALSE);
    check_oal_error("alSourcei AL_LOOPING");
    alDistanceModel(AL_INVERSE_DISTANCE);
    check_oal_error("alDistanceModel");

    // запуск звука
    alSourcePlay(audio_info.oal_source_id);
    check_oal_error("alSourcePlay");

    auto id = make_id();
    audio_info.audio_id = id;
    bind_audio(id, std::move(audio_info));
    return id;
  } // play

  inline void make_update_thread() {
    m_update_thread_live = true;
    m_update_thread = std::thread( [this] {
      log_debug << "Sound_mgr_oal: start update thread";

      while (true) {
        return_if (!m_update_thread_live);

        {
          std::lock_guard lock(m_mutex);
          update();
        }

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(50ms);
      }
      
      log_debug << "Sound_mgr_oal: end of update thread";
    } );
  }

  inline void set_listener_pos(const Vec3 listener_pos) {
    std::lock_guard lock(m_mutex);
    alListener3f(AL_POSITION, listener_pos.x, listener_pos.y, listener_pos.z);
    check_oal_error("alListener3f AL_POSITION");
  }

  inline void set_master_gain(const real gain) {
    std::lock_guard lock(m_mutex);
    alListenerf(AL_GAIN, gain);
    check_oal_error("alListenerf AL_GAIN");
  }
  
  inline bool is_playing(const Audio_id sound_id) const {
    return_if (sound_id == BAD_AUDIO, false);
    std::lock_guard lock(m_mutex);
    // узнать что звук есть
    auto finded_audio_info = m_audio_infos.find(sound_id);
    if (finded_audio_info == m_audio_infos.end())
      return false;
    // узнать у OAL что звук проигрывается
    ALint state;
    alGetSourcei(finded_audio_info->second.oal_source_id, AL_SOURCE_STATE, &state);
    return state == AL_PLAYING;
  }

  inline void stop(const Audio_id sound_id) {
    if ( !is_playing(sound_id)) {
      log_warning << "попытка остановить звук, который уже не играет (ID: " + n2s(sound_id) + ")";
      return;
    }

    std::lock_guard lock(m_mutex);
    auto finded_audio_info = m_audio_infos.find(sound_id);
    if (finded_audio_info != m_audio_infos.end()) {
      cauto source = finded_audio_info->second.oal_source_id;
      ALint state;

      alGetSourcei(finded_audio_info->second.oal_source_id, AL_SOURCE_STATE, &state);
      check_oal_error("alGetSourcei AL_SOURCE_STATE");

      if (state == AL_PLAYING) {
        alSourcePause(source);
        check_oal_error("alSourcePause source");
      }
    }
  }

  inline void continue_play(const Audio_id sound_id) {
    std::lock_guard lock(m_mutex);
    auto finded_audio_info = m_audio_infos.find(sound_id);
    if (finded_audio_info != m_audio_infos.end()) {
      cauto source = finded_audio_info->second.oal_source_id;
      ALint state;

      alGetSourcei(source, AL_SOURCE_STATE, &state);
      check_oal_error("alGetSourcei AL_SOURCE_STATE");

      if (state == AL_PAUSED) {
        alSourcePlay(source);
        check_oal_error("alSourcePlay source");
      }
    }
  }

  inline void check_audio_ctx(const Audio_id sound_id) const {
    iferror(sound_id == BAD_AUDIO, "bad audio ID");
  }

  inline void set_amplify(const Audio_id sound_id, const real amplify) {
    if ( !is_playing(sound_id)) {
      log_warning << "попытка остановить звук, который уже не играет (ID: "
        + n2s(sound_id) + ")";
      return;
    }

    std::lock_guard lock(m_mutex);
    error("need impl");
  }

  inline void set_position(const Audio_id sound_id, const Vec3 new_pos) {
    if ( !is_playing(sound_id)) {
      log_warning << "попытка изменить позицию звуку, который уже не играет (ID: "
        + n2s(sound_id) + ")";
      return;
    }
    std::lock_guard lock(m_mutex);
    auto finded_audio_info = m_audio_infos.find(sound_id);
    if (finded_audio_info != m_audio_infos.end()) {
      alSource3f(finded_audio_info->second.oal_source_id, AL_POSITION, new_pos.x, new_pos.y, new_pos.z);
      check_oal_error("alSource3f AL_POSITION");
    }
  }

  inline void set_velocity(const Audio_id sound_id, const Vec3 new_vel) {
    if ( !is_playing(sound_id)) {
      log_warning << "попытка изменить скорость движения звуку, который уже не играет (ID: "
        + n2s(sound_id) + ")";
      return;
    }
    std::lock_guard lock(m_mutex);
    auto finded_audio_info = m_audio_infos.find(sound_id);
    if (finded_audio_info != m_audio_infos.end()) {
      alSource3f(finded_audio_info->second.oal_source_id, AL_VELOCITY, new_vel.x, new_vel.y, new_vel.z);
      check_oal_error("alSource3f AL_VELOCITY");
    }
  }

  inline void add_audio(cr<Str> sound_name, cr<Audio> sound) {
    check_audio(sound);
    std::lock_guard lock(m_mutex);
    m_store[sound_name] = sound;
  }

  inline void move_audio(cr<Str> sound_name, Audio&& sound) {
    check_audio(sound);
    std::lock_guard lock(m_mutex);
    m_store[sound_name] = std::move(sound);
  }

  void set_pitch(const Audio_id sound_id, const real pitch) {
    if ( !is_playing(sound_id)) {
      log_warning << "попытка изменить тон звуку, который уже не играет (ID: "
        + n2s(sound_id) + ")";
    }
    std::lock_guard lock(m_mutex);
    auto finded_audio_info = m_audio_infos.find(sound_id);
    if (finded_audio_info != m_audio_infos.end()) {
      alSourcef(finded_audio_info->second.oal_source_id, AL_PITCH, pitch);
      check_oal_error("alSourcef AL_PITCH");
    }
  }

  inline Audio_id make_id() {
    ++m_uid;
    // скпинуть bad audio id
    if (m_uid == BAD_AUDIO)
      ++m_uid;
    return m_uid;
  }

  // проверить что звук корректно создан
  inline void check_audio(cr<Audio> sound) const {
    assert(sound.channels != 0);
    assert(sound.channels <= 8);
    assert(sound.compression != Audio::Compression::opus); // need impl
    assert(sound.compression != Audio::Compression::mp3); // need impl
    assert(!sound.data.empty());
    assert(sound.samples != 0);
    assert(sound.frequency != 0);
  }

  inline cr<Audio> find_audio(cr<Str> sound_name) const {
    try {
      std::lock_guard lock(m_mutex);
      return m_store.at(sound_name);
    } catch(...) {
      error("Sound \"" << sound_name << "\" not finded in Sound Manager");
    }
    return *rcast<Audio*>(0); // заглушка для анализатора
  }

  inline void init_openal() {
    log_debug << "init OpenAL";
    // открыть аудио устройство по умолчанию (TODO сделать выбор)
    auto oal_device = alcOpenDevice({});
    iferror(!oal_device, "alcOpenDevice error " + decode_oal_error(alGetError()));
    auto oal_context = alcCreateContext(oal_device, {});
    alcMakeContextCurrent(oal_context);
    // Check for EAX 2.0 support
    m_eax_2_0_compat = alIsExtensionPresent("EAX2.0"); 
    log_debug << Str("EAX 2.0 support: ") + s2yn(m_eax_2_0_compat);
    alGetError(); // clear error code 
  } // init_openal

  inline void close_oal() {
    log_debug << "disable OpenAL\n";
    for (crauto audio_info: m_audio_infos) {
      disable(audio_info.first);
      alDeleteSources(1, &audio_info.second.oal_source_id);
      alDeleteBuffers(m_config.buffers, audio_info.second.oal_buffer_ids.data());
    }
    auto oal_context = alcGetCurrentContext();
    auto oal_device = alcGetContextsDevice(oal_context);
    alcMakeContextCurrent({});
    alcDestroyContext(oal_context);
    alcCloseDevice(oal_device); 
  }

  // выключить трек
  inline void disable(const Audio_id sound_id) {
    if (is_playing(sound_id)) {
      std::lock_guard lock(m_mutex);
      auto finded_audio_info = m_audio_infos.find(sound_id);
      iferror(finded_audio_info == m_audio_infos.end(), "sound id " << sound_id << " not found");
      alSourceStop(finded_audio_info->second.oal_source_id);
      check_oal_error("alSourceStop");
      alSourceRewind(finded_audio_info->second.oal_source_id);
      check_oal_error("alSourceRewind");
      alSourcei(finded_audio_info->second.oal_source_id, AL_BUFFER, 0);
      check_oal_error("alSourcei AL_BUFFER set 0");
    }
  }

  using Sound_buffer_converter = std::function<Bytes (cr<Bytes>, const std::size_t)>;

  struct Format_game_to_oal {
    uint channel {};
    Audio::Format sound_format {};
    ALenum oal_format {};
    Sound_buffer_converter converter {};
    uint bytes_per_sample {};
  };

  inline static Bytes conv_mono_f32_to_s16(cr<Bytes> src, const std::size_t samples) {
    return_if (samples == 0, {});
    using src_t = float;
    using dst_t = std::int16_t;
    Bytes ret(samples * sizeof(dst_t));
    auto dst_p = rcast<dst_t*>(ret.data());
    auto src_p = rcast<cp<src_t>>(src.data());

    cfor (i, samples)
      dst_p[i] = src_p[i] * 0x7FFF;

    return ret;
  }

  inline static Bytes conv_mono_s16_to_s16(cr<Bytes> src, const std::size_t samples) { return src; }
  inline static Bytes conv_mono_u8_to_u8(cr<Bytes> src, const std::size_t samples) { return src; }

  inline static Bytes conv_stereo_f32_to_s16(cr<Bytes> src, const std::size_t samples) {
    return_if (samples == 0, {});
    using src_t = float;
    using dst_t = std::int16_t;
    Bytes ret(samples * 2 * sizeof(dst_t));
    auto dst_p = rcast<dst_t*>(ret.data());
    auto src_p = rcast<cp<src_t>>(src.data());

    cfor (i, samples * 2)
      dst_p[i] = src_p[i] * 0x7FFF;

    return ret;
  }

  inline static Bytes conv_stereo_s16_to_s16(cr<Bytes> src, const std::size_t samples) { return src; }
  inline static Bytes conv_stereo_u8_to_u8(cr<Bytes> src, const std::size_t samples) { return src; }

  inline cr<Format_game_to_oal> to_compatible_oal_format(cr<Audio> sound) const {
    static const Vector<Format_game_to_oal> format_table {
      {.channel = 1, .sound_format = Audio::Format::pcm_f32,     .oal_format = AL_FORMAT_MONO16,   .converter = &conv_mono_f32_to_s16,   .bytes_per_sample = 4},
      {.channel = 1, .sound_format = Audio::Format::raw_pcm_s16, .oal_format = AL_FORMAT_MONO16,   .converter = &conv_mono_s16_to_s16,   .bytes_per_sample = 2},
      {.channel = 1, .sound_format = Audio::Format::raw_pcm_u8,  .oal_format = AL_FORMAT_MONO8,    .converter = &conv_mono_u8_to_u8,     .bytes_per_sample = 1},
      {.channel = 2, .sound_format = Audio::Format::pcm_f32,     .oal_format = AL_FORMAT_STEREO16, .converter = &conv_stereo_f32_to_s16, .bytes_per_sample = 4},
      {.channel = 2, .sound_format = Audio::Format::raw_pcm_s16, .oal_format = AL_FORMAT_STEREO16, .converter = &conv_stereo_s16_to_s16, .bytes_per_sample = 2},
      {.channel = 2, .sound_format = Audio::Format::raw_pcm_u8,  .oal_format = AL_FORMAT_STEREO8,  .converter = &conv_stereo_u8_to_u8,   .bytes_per_sample = 1},
    };

    for (crauto format_info: format_table) {
      if (sound.channels == format_info.channel && sound.format == format_info.sound_format)
        return format_info;
    }

    error("not finded OAL format for sound \"" << sound.get_path() << "\"");
    return *rcast<Format_game_to_oal*>(0);
  }

  // записать звуковой источник в список проигрывания
  inline void bind_audio(const Audio_id sound_id, Audio_info&& info) {
    assert(sound_id != BAD_AUDIO);
    m_audio_infos[sound_id] = std::move(info);
  }

  inline void set_doppler_factor(const real doppler_factor) {
    std::lock_guard lock(m_mutex);
    alDopplerFactor(doppler_factor); // настройка эффекта Допплера
    check_oal_error("alDopplerFactor");
  }

  inline void update() {
    // обновить буферы
    for (rauto audio_info: m_audio_infos)
      audio_info.second.update(this);

    // удалить из списка все треки, которые уже не играют
    std::erase_if (
      m_audio_infos,
      [this](cr<decltype(m_audio_infos)::value_type> audio_info_pair)->bool {
        crauto audio_info = audio_info_pair.second; // allias
        ALint state;
        alGetSourcei(audio_info.oal_source_id, AL_SOURCE_STATE, &state);
        
        if (state == AL_STOPPED) {
          alDeleteSources(1, &audio_info.oal_source_id);
          alDeleteBuffers(m_config.buffers, audio_info.oal_buffer_ids.data());
          return true;
        }

        return false;
      }
    );
  } // update

  static inline Shared<Packet_decoder> make_packet_decoder(cr<Audio> sound) {
    switch (sound.compression) {
      default:
      case Audio::Compression::raw: return new_shared<Packet_decoder_raw>(sound); break;
      case Audio::Compression::flac: return new_shared<Packet_decoder_flac>(sound); break;
      case Audio::Compression::vorbis: return new_shared<Packet_decoder_vorbis>(sound); break;
      case Audio::Compression::opus: return new_shared<Packet_decoder_opus>(sound); break;
      case Audio::Compression::mp3: error("need impl for mp3 decoder"); break;
    }
    error("packet decoder not created");
    return {};
  }

  inline void shutup() {
    Vector<Audio_id> ids_for_shutup {};
    // буфферизировать идентификаторы управления
    {
      std::lock_guard lock(m_mutex);
      for (crauto audio_info: m_audio_infos)
        ids_for_shutup.push_back(audio_info.first);
    }
    // выключить все звуки
    for (cauto id: ids_for_shutup)
      disable(id);
  }

  void stop_all() {
    for (crauto audio_info: m_audio_infos)
      stop(audio_info.second.audio_id);
  }

  void continue_all() {
    for (crauto audio_info: m_audio_infos)
      continue_play(audio_info.second.audio_id);
  }
}; // Impl

Strs parse_raw_list(Cstr raw_list) {
  Strs ret;
  while (raw_list[0] != '\0') {
    const auto len = std::strlen(raw_list);
    ret.push_back(Str(raw_list, raw_list + len));
    raw_list += len + 1;
  }
  return ret;
}

[[nodiscard]] Vector<Device_name> get_audio_devices() {
  try {
    Vector<Device_name> ret;
    cauto enumeration = alcIsExtensionPresent({}, "ALC_ENUMERATION_EXT");
    check_oal_error("alcIsExtensionPresent ALC_ENUMERATION_EXT");
    if (enumeration == AL_FALSE) {
      error("OAL device enumeration not supported");
    } else {
      Cstr device_list_raw;
      if (alcIsExtensionPresent({}, "ALC_enumerate_all_EXT") == AL_FALSE)
        device_list_raw = rcast<Cstr>( alcGetString({}, ALC_DEVICE_SPECIFIER) );
      else
        device_list_raw = rcast<Cstr>( alcGetString({}, ALC_ALL_DEVICES_SPECIFIER) );
      cauto device_list = parse_raw_list(device_list_raw);
      cfor (i, device_list.size()) {
        ret.emplace_back( Device_name{.id=i, .name=device_list[i]} );
      }
    }
    return ret;
  } catch (...) {
    log_info << ("audio devices not finded");
  }
  return {};
} // get_audio_devices

Sound_mgr_oal::Sound_mgr_oal(cr<Sound_mgr_config> config): impl {new_unique<Impl>(config)} {}
Sound_mgr_oal::~Sound_mgr_oal() {}
Audio_id Sound_mgr_oal::play(cr<Str> sound_name, const Vec3 source_position, const Vec3 source_velocity,
const real amplify, const bool repeat)
  { return impl->play(sound_name, source_position, source_velocity, amplify, repeat); }
void Sound_mgr_oal::set_listener_pos(const Vec3 listener_pos) { impl->set_listener_pos(listener_pos); }
bool Sound_mgr_oal::is_playing(const Audio_id sound_id) const { return impl->is_playing(sound_id); }
void Sound_mgr_oal::set_amplify(const Audio_id sound_id, const real amplify) { impl->set_amplify(sound_id, amplify); }
void Sound_mgr_oal::set_position(const Audio_id sound_id, const Vec3 new_pos) { impl->set_position(sound_id, new_pos); }
void Sound_mgr_oal::set_velocity(const Audio_id sound_id, const Vec3 new_vel) { impl->set_velocity(sound_id, new_vel); }
void Sound_mgr_oal::stop(const Audio_id sound_id) { impl->stop(sound_id); }
void Sound_mgr_oal::continue_play(const Audio_id sound_id) { impl->continue_play(sound_id); }
void Sound_mgr_oal::add_audio(cr<Str> sound_name, cr<Audio> sound) { impl->add_audio(sound_name, sound); }
void Sound_mgr_oal::move_audio(cr<Str> sound_name, Audio&& sound) { impl->move_audio(sound_name, std::move(sound)); }
void Sound_mgr_oal::set_pitch(const Audio_id sound_id, const real pitch) { impl->set_pitch(sound_id, pitch); }
void Sound_mgr_oal::set_master_gain(const real gain) { impl->set_master_gain(gain); }
void Sound_mgr_oal::set_doppler_factor(const real doppler_factor) { impl->set_doppler_factor(doppler_factor); }
void Sound_mgr_oal::disable(const Audio_id sound_id) { impl->disable(sound_id); }
cr<Audio> Sound_mgr_oal::find_audio(cr<Str> sound_name) const { return impl->find_audio(sound_name); }
void Sound_mgr_oal::shutup() { impl->shutup(); }
void Sound_mgr_oal::stop_all() { impl->stop_all(); }
void Sound_mgr_oal::continue_all() { impl->continue_all(); }

cr<Audio> Sound_mgr_nosound::find_audio(cr<Str> sound_name) const {
  error("called find_audio in nosound mode");
  return *rcast<Audio*>(0); // заглушка для анализатора
}
