#include <OpenAL-soft/AL/al.h>
#include <OpenAL-soft/AL/alc.h>
#include <atomic>
#include <thread>
#include <cstdint>
#include <cassert>
#include <functional>
#include <utility>
#include <unordered_map>
#include "sound-manager.hpp"
#include "packet-decoder.hpp"
#include "util/log.hpp"
#include "util/error.hpp"

struct Sound_mgr::Impl {
  Sound_mgr_config m_config {};

  struct Audio_info {
    Vector<ALuint> oal_buffer_ids {};
    ALuint oal_source_id {};
    Shared<Packet_decoder> packet_decoder {}; // выдаёт поблочно данные аудио файла
    CP<Audio> sound {}; // прикреплённый звук из банка

    inline void update(Impl* master) {
      assert(packet_decoder);
      assert(sound);
      assert(oal_buffer_ids.size() >= master->m_config.buffers);

      // узнать состояние буфферов
      ALint buffers_processed = 0;
      alGetSourcei(oal_source_id, AL_BUFFERS_PROCESSED, &buffers_processed);
      check_oal_error("alGetSourcei AL_BUFFERS_PROCESSED");

      cnauto format = master->to_compatible_oal_format(*sound);

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

  inline explicit Impl(CN<Sound_mgr_config> config): m_config {config} {
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

  inline Audio_id play(CN<Str> sound_name, const Vec3 source_position,
  const Vec3 source_velocity, const real amplify, const bool repeat) {
    if (m_audio_infos.size() >= m_config.sounds) {
      detailed_log("no more OAL buffer/source\n");
      return BAD_AUDIO;
    }

    cnauto sound = find_audio(sound_name);
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
    cnauto format = to_compatible_oal_format(*audio_info.sound);
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
    bind_audio(id, std::move(audio_info));
    return id;
  } // play

  inline void make_update_thread() {
    m_update_thread_live = true;
    m_update_thread = std::thread( [this] {
      detailed_log("Sound_mgr: start update thread\n");
      while (m_update_thread_live.load(std::memory_order::relaxed)) {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(50ms);
        std::lock_guard lock(m_mutex);
        update();
      }
      detailed_log("Sound_mgr: end of update thread\n");
    } );
  }

  static inline void check_oal_error(CN<Str> func_name) {
    if (cauto error = alGetError(); error != AL_NO_ERROR)
      error("error in OAL function: " + func_name + " - " + decode_oal_error(error));
  }

  inline void set_listener_pos(const Vec3 listener_pos) {
    std::lock_guard lock(m_mutex);
    alListener3f(AL_POSITION, listener_pos.x, listener_pos.y, listener_pos.z);
    check_oal_error("alListener3f AL_POSITION");
  }

  inline void set_master_gain(const float gain) {
    std::lock_guard lock(m_mutex);
    alListenerf(AL_GAIN, gain);
    check_oal_error("alListenerf AL_GAIN");
  }
  
  inline void set_listener_dir(const Vec3 listener_dir) {
    const float ori [3] {listener_dir.x, listener_dir.y, listener_dir.z};
    std::lock_guard lock(m_mutex);
    alListenerfv(AL_ORIENTATION, ori);
    check_oal_error("alListener3f AL_ORIENTATION");
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
      detailed_log("попытка остановить звук, который уже не играет (ID: "
        << sound_id << ")\n");
      return;
    }
    std::lock_guard lock(m_mutex);
    // TODO
  }

  inline void check_audio_ctx(const Audio_id sound_id) const {
    iferror(sound_id == BAD_AUDIO, "bad audio ID");
  }

  inline void set_amplify(const Audio_id sound_id, const real amplify) {
    if ( !is_playing(sound_id)) {
      detailed_log("попытка остановить звук, который уже не играет (ID: "
        << sound_id << ")\n");
      return;
    }
    std::lock_guard lock(m_mutex);
    // TODO
  }

  inline void set_position(const Audio_id sound_id, const Vec3 new_pos) {
    if ( !is_playing(sound_id)) {
      detailed_log (
        "попытка изменить позицию звуку, который уже не играет (ID: "
        << sound_id << ")\n");
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
      detailed_log (
        "попытка изменить скорость движения звуку, который уже не играет (ID: "
        << sound_id << ")\n");
      return;
    }
    std::lock_guard lock(m_mutex);
    auto finded_audio_info = m_audio_infos.find(sound_id);
    if (finded_audio_info != m_audio_infos.end()) {
      alSource3f(finded_audio_info->second.oal_source_id, AL_VELOCITY, new_vel.x, new_vel.y, new_vel.z);
      check_oal_error("alSource3f AL_VELOCITY");
    }
  }

  inline void add_audio(CN<Str> sound_name, CN<Audio> sound) {
    check_audio(sound);
    std::lock_guard lock(m_mutex);
    m_store[sound_name] = sound;
  }

  inline void move_audio(CN<Str> sound_name, Audio&& sound) {
    check_audio(sound);
    std::lock_guard lock(m_mutex);
    m_store[sound_name] = std::move(sound);
  }

  void set_pitch(const Audio_id sound_id, const float pitch) {
    if ( !is_playing(sound_id)) {
      detailed_log (
        "попытка изменить тон звуку, который уже не играет (ID: "
        << sound_id << ")\n");
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
  inline void check_audio(CN<Audio> sound) const {
    assert(sound.channels != 0);
    assert(sound.channels <= 8);
    assert(sound.compression != Audio::Compression::opus); // need impl
    assert(sound.compression != Audio::Compression::mp3); // need impl
    assert(!sound.data.empty());
    assert(sound.samples != 0);
    assert(sound.frequency != 0);
  }

  inline CN<Audio> find_audio(CN<Str> sound_name) const {
    try {
      std::lock_guard lock(m_mutex);
      return m_store.at(sound_name);
    } catch(...) {
      error("Sound \"" << sound_name << "\" not finded in Sound Manager");
    }
    return *(Audio*)0; // заглушка для анализатора
  }

  inline void init_openal() {
    detailed_log("init OpenAL\n");
    // открыть аудио устройство по умолчанию (TODO сделать выбор)
    auto oal_device = alcOpenDevice({});
    iferror(!oal_device, "alcOpenDevice error " + decode_oal_error(alGetError()));
    auto oal_context = alcCreateContext(oal_device, {});
    alcMakeContextCurrent(oal_context);
    // Check for EAX 2.0 support
    m_eax_2_0_compat = alIsExtensionPresent("EAX2.0"); 
    detailed_log("EAX 2.0 support: " << s2yn(m_eax_2_0_compat) << '\n');
    alGetError(); // clear error code 
  } // init_openal

  inline void close_oal() {
    detailed_log("disable OpenAL\n");
    for (cnauto audio_info: m_audio_infos) {
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

  static inline Str decode_oal_error(const ALenum error_enum) {
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

  using Sound_buffer_converter = std::function<Bytes (CN<Bytes>, const std::size_t)>;

  struct Format_game_to_oal {
    uint channel {};
    Audio::Format sound_format {};
    ALenum oal_format {};
    Sound_buffer_converter converter {};
    uint bytes_per_sample {};
  };

  inline static Bytes conv_mono_f32_to_s16(CN<Bytes> src, const std::size_t samples) {
    return_if (samples == 0, {});
    using src_t = float;
    using dst_t = std::int16_t;
    Bytes ret(samples * sizeof(dst_t));
    auto dst_p = rcast<dst_t*>(ret.data());
    auto src_p = rcast<CP<src_t>>(src.data());
    cfor (i, samples)
      dst_p[i] = src_p[i] * 0x7FFF;
    return ret;
  }

  inline static Bytes conv_mono_s16_to_s16(CN<Bytes> src, const std::size_t samples) { return src; }
  inline static Bytes conv_mono_u8_to_u8(CN<Bytes> src, const std::size_t samples) { return src; }

  inline static Bytes conv_stereo_f32_to_s16(CN<Bytes> src, const std::size_t samples) {
    return_if (samples == 0, {});
    using src_t = float;
    using dst_t = std::int16_t;
    Bytes ret(samples * 2 * sizeof(dst_t));
    auto dst_p = rcast<dst_t*>(ret.data());
    auto src_p = rcast<CP<src_t>>(src.data());
    cfor (i, samples * 2)
      dst_p[i] = src_p[i] * 0x7FFF;
    return ret;
  }

  inline static Bytes conv_stereo_s16_to_s16(CN<Bytes> src, const std::size_t samples) { return src; }
  inline static Bytes conv_stereo_u8_to_u8(CN<Bytes> src, const std::size_t samples) { return src; }

  inline CN<Format_game_to_oal> to_compatible_oal_format(CN<Audio> sound) const {
    static const Vector<Format_game_to_oal> format_table {
      {.channel = 1, .sound_format = Audio::Format::pcm_f32,     .oal_format = AL_FORMAT_MONO16,   .converter = &conv_mono_f32_to_s16,   .bytes_per_sample = 4},
      {.channel = 1, .sound_format = Audio::Format::raw_pcm_s16, .oal_format = AL_FORMAT_MONO16,   .converter = &conv_mono_s16_to_s16,   .bytes_per_sample = 2},
      {.channel = 1, .sound_format = Audio::Format::raw_pcm_u8,  .oal_format = AL_FORMAT_MONO8,    .converter = &conv_mono_u8_to_u8,     .bytes_per_sample = 1},
      {.channel = 2, .sound_format = Audio::Format::pcm_f32,     .oal_format = AL_FORMAT_STEREO16, .converter = &conv_stereo_f32_to_s16, .bytes_per_sample = 4},
      {.channel = 2, .sound_format = Audio::Format::raw_pcm_s16, .oal_format = AL_FORMAT_STEREO16, .converter = &conv_stereo_s16_to_s16, .bytes_per_sample = 2},
      {.channel = 2, .sound_format = Audio::Format::raw_pcm_u8,  .oal_format = AL_FORMAT_STEREO8,  .converter = &conv_stereo_u8_to_u8,   .bytes_per_sample = 1},
    };
    for (cnauto format_info: format_table) {
      if (sound.channels == format_info.channel && sound.format == format_info.sound_format)
        return format_info;
    }
    error("not finded OAL format for sound \"" << sound.get_path() << "\"");
    return *(Format_game_to_oal*)0;
  }

  // записать звуковой источник в список проигрывания
  inline void bind_audio(const Audio_id sound_id, Audio_info&& info) {
    assert(sound_id != BAD_AUDIO);
    m_audio_infos[sound_id] = std::move(info);
  }

  inline void set_doppler_factor(const float doppler_factor) {
    std::lock_guard lock(m_mutex);
    alDopplerFactor(doppler_factor); // настройка эффекта Допплера
    check_oal_error("alDopplerFactor");
  }

  inline void update() {
    // обновить буферы
    for (nauto audio_info: m_audio_infos)
      audio_info.second.update(this);
    // удалить из списка все треки, которые уже не играют
    std::erase_if (
      m_audio_infos,
      [this](const decltype(m_audio_infos)::value_type audio_info)->bool {
        ALint state;
        alGetSourcei(audio_info.second.oal_source_id, AL_SOURCE_STATE, &state);
        
        if (state != AL_PLAYING) {
          alDeleteSources(1, &audio_info.second.oal_source_id);
          alDeleteBuffers(m_config.buffers, audio_info.second.oal_buffer_ids.data());
          return true;
        }
        return false;
      }
    );
  } // update

  static inline Shared<Packet_decoder> make_packet_decoder(CN<Audio> sound) {
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
}; // Impl

Sound_mgr::Sound_mgr(CN<Sound_mgr_config> config): impl {new_unique<Impl>(config)} {}
Sound_mgr::~Sound_mgr() {}
Audio_id Sound_mgr::play(CN<Str> sound_name, const Vec3 source_position, const Vec3 source_velocity,
const real amplify, const bool repeat)
  { return impl->play(sound_name, source_position, source_velocity, amplify, repeat); }
void Sound_mgr::set_listener_pos(const Vec3 listener_pos) { impl->set_listener_pos(listener_pos); }
void Sound_mgr::set_listener_dir(const Vec3 listener_dir) { impl->set_listener_dir(listener_dir); }
bool Sound_mgr::is_playing(const Audio_id sound_id) const { return impl->is_playing(sound_id); }
void Sound_mgr::set_amplify(const Audio_id sound_id, const real amplify) { impl->set_amplify(sound_id, amplify); }
void Sound_mgr::set_position(const Audio_id sound_id, const Vec3 new_pos) { impl->set_position(sound_id, new_pos); }
void Sound_mgr::set_velocity(const Audio_id sound_id, const Vec3 new_vel) { impl->set_velocity(sound_id, new_vel); }
void Sound_mgr::stop(const Audio_id sound_id) { impl->stop(sound_id); }
void Sound_mgr::add_audio(CN<Str> sound_name, CN<Audio> sound) { impl->add_audio(sound_name, sound); }
void Sound_mgr::move_audio(CN<Str> sound_name, Audio&& sound) { impl->move_audio(sound_name, std::move(sound)); }
void Sound_mgr::set_pitch(const Audio_id sound_id, const float pitch) { impl->set_pitch(sound_id, pitch); }
void Sound_mgr::set_master_gain(const float gain) { impl->set_master_gain(gain); }
void Sound_mgr::set_doppler_factor(const float doppler_factor) { impl->set_doppler_factor(doppler_factor); }
void Sound_mgr::disable(const Audio_id sound_id) { impl->disable(sound_id); }
