#include <cassert>
#include <cstring>
#include <algorithm>
#define STB_VORBIS_HEADER_ONLY
#include <stb/stb_vorbis.c>
#include "packet-decoder.hpp"
#include "audio.hpp"

Packet_decoder_flac::~Packet_decoder_flac() {}
Packet_decoder_opus::~Packet_decoder_opus() {}
Packet_decoder_vorbis::~Packet_decoder_vorbis() {}
Packet_decoder_raw::~Packet_decoder_raw() {}

struct Packet_decoder_flac::Impl {};
Packet_decoder_flac::Packet_decoder_flac(CN<Audio> sound) {}
Bytes Packet_decoder_flac::decode(const uint needed_sz) { return {}; }

struct Packet_decoder_opus::Impl {};
Packet_decoder_opus::Packet_decoder_opus(CN<Audio> sound) {}
Bytes Packet_decoder_opus::decode(const uint needed_sz) { return {}; }

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
    Bytes ret(needed_bytes);
    memcpy(ret.data(), m_sound->data.data() + m_readed_bytes, needed_bytes);
    m_readed_bytes += needed_bytes;
    return ret;
  }
};

Packet_decoder_raw::Packet_decoder_raw(CN<Audio> sound): impl {new_unique<Impl>(sound)} {}
Bytes Packet_decoder_raw::decode(const uint needed_sz) { return impl->decode(needed_sz); }

struct Packet_decoder_vorbis::Impl {};
Packet_decoder_vorbis::Packet_decoder_vorbis(CN<Audio> sound) {}
Bytes Packet_decoder_vorbis::decode(const uint needed_sz) { return {}; }
