#include <dr_libs/dr_flac.h>
#include <cassert>
#include <cstring>
#include <algorithm>
#include "packet-decoder.hpp"
#include "audio.hpp"
#include "util/error.hpp"
#define STB_VORBIS_HEADER_ONLY
#include <stb/stb_vorbis.c>

Packet_decoder_flac::~Packet_decoder_flac() {}
Packet_decoder_opus::~Packet_decoder_opus() {}
Packet_decoder_vorbis::~Packet_decoder_vorbis() {}
Packet_decoder_raw::~Packet_decoder_raw() {}

struct Packet_decoder_opus::Impl {};
Packet_decoder_opus::Packet_decoder_opus(CN<Audio> sound) {}
Bytes Packet_decoder_opus::decode(const uint needed_sz) { return {}; }

struct Packet_decoder_flac::Impl {
  CP<Audio> m_sound {};
  drflac* m_flac_info {};

  inline explicit Impl(CN<Audio> sound): m_sound {&sound} {
    assert(m_sound);
    assert(!m_sound->data.empty());
    m_flac_info = drflac_open_memory(m_sound->data.data(), m_sound->data.size(), {});
  }

  inline ~Impl() { drflac_free(m_flac_info, {}); }

  inline Bytes decode(const uint needed_sz) {
    switch (m_sound->format) {
      case Audio::Format::pcm_f32: {
        using sample_t = float;
        Bytes ret(needed_sz * sizeof(sample_t) * m_sound->channels, 0);
        cauto count = drflac_read_pcm_frames_f32 (
          m_flac_info, needed_sz, rcast<float*>(ret.data()) );
        ret.resize(count * sizeof(sample_t) * m_sound->channels);
        return ret;
      }
      case Audio::Format::raw_pcm_s16: {
        using sample_t = std::int16_t;
        Bytes ret(needed_sz * sizeof(sample_t) * m_sound->channels, 0);
        cauto count = drflac_read_pcm_frames_s16 (
          m_flac_info, needed_sz, rcast<drflac_int16*>(ret.data()) );
        ret.resize(count * sizeof(sample_t) * m_sound->channels);
        return ret;
      }
      default:
        error("unknown FLAC sample format");
    } // switch format
    return {};
  } // decode
}; // FLAC impl

Packet_decoder_flac::Packet_decoder_flac(CN<Audio> sound): impl {new_unique<Impl>(sound)} {}
Bytes Packet_decoder_flac::decode(const uint needed_sz) { return impl->decode(needed_sz); }

struct Packet_decoder_raw::Impl {
  std::size_t m_readed_bytes {};
  std::size_t m_size {};
  CP<Audio> m_sound {};

  inline explicit Impl(CN<Audio> sound)
  : m_size {sound.data.size()}
  , m_sound {&sound}
  {
    assert(m_size > 0);
    assert(m_sound);
  }

  inline Bytes decode(const uint needed_sz) {
    // не читать больше данных, чем есть в m_sound
    uint needed_bytes = needed_sz;
    if (m_readed_bytes + needed_sz >= m_sound->data.size())
      needed_bytes = m_sound->data.size() - m_readed_bytes;
    if (needed_bytes == 0)
      return {};
    // скопировать блок данных
    Bytes ret(needed_sz);
    memcpy(ret.data(), m_sound->data.data() + m_readed_bytes, needed_bytes);
    m_readed_bytes += needed_bytes;
    return ret;
  }
}; // RAW impl

Packet_decoder_raw::Packet_decoder_raw(CN<Audio> sound): impl {new_unique<Impl>(sound)} {}
Bytes Packet_decoder_raw::decode(const uint needed_sz) { return impl->decode(needed_sz); }

struct Packet_decoder_vorbis::Impl {
  CP<Audio> m_sound {};
  stb_vorbis* m_vorbis_file {};

  inline explicit Impl(CN<Audio> sound): m_sound {&sound} {
    assert(m_sound);
    int error;
    m_vorbis_file = stb_vorbis_open_memory(rcast<const unsigned char*>(m_sound->data.data()), 
      m_sound->data.size(), &error, {});
    assert(m_sound->format == Audio::Format::pcm_f32);
  }

  inline ~Impl() { stb_vorbis_close(m_vorbis_file); }

  inline Bytes decode(const uint needed_sz) {
    using sample_t = float;
    Bytes ret(needed_sz * sizeof(sample_t), 0);
    cauto count = stb_vorbis_get_samples_float_interleaved(m_vorbis_file,
      m_sound->channels, rcast<sample_t*>(ret.data()), needed_sz);
    ret.resize(count * sizeof(sample_t));
    return ret;
  }
}; // Vorbis impl

Packet_decoder_vorbis::Packet_decoder_vorbis(CN<Audio> sound): impl {new_unique<Impl>(sound)} {}
Bytes Packet_decoder_vorbis::decode(const uint needed_sz) { return impl->decode(needed_sz); }
