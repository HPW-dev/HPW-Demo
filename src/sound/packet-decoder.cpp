#define STB_VORBIS_HEADER_ONLY
#include <stb/stb_vorbis.c>
#include "packet-decoder.hpp"

Packet_decoder_flac::~Packet_decoder_flac() {}
Packet_decoder_opus::~Packet_decoder_opus() {}
Packet_decoder_vorbis::~Packet_decoder_vorbis() {}
Packet_decoder_raw::~Packet_decoder_raw() {}

struct Packet_decoder_flac::Packet_decoder_flac::Impl {};
Packet_decoder_flac::Packet_decoder_flac(CN<Audio> sound) {}
Bytes Packet_decoder_flac::decode(const uint needed_sz) { return {}; }

struct Packet_decoder_opus::Packet_decoder_opus::Impl {};
Packet_decoder_opus::Packet_decoder_opus(CN<Audio> sound) {}
Bytes Packet_decoder_opus::decode(const uint needed_sz) { return {}; }

struct Packet_decoder_raw::Packet_decoder_raw::Impl {};
Packet_decoder_raw::Packet_decoder_raw(CN<Audio> sound) {}
Bytes Packet_decoder_raw::decode(const uint needed_sz) { return {}; }

struct Packet_decoder_vorbis::Packet_decoder_vorbis::Impl {};
Packet_decoder_vorbis::Packet_decoder_vorbis(CN<Audio> sound) {}
Bytes Packet_decoder_vorbis::decode(const uint needed_sz) { return {}; }
