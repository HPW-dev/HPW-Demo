#include "packet-decoder.hpp"

Packet_decoder_flac::~Packet_decoder_flac() {}
Packet_decoder_opus::~Packet_decoder_opus() {}
Packet_decoder_vorbis::~Packet_decoder_vorbis() {}
Packet_decoder_raw::~Packet_decoder_raw() {}

struct Packet_decoder_flac::Packet_decoder_flac::Impl {};
Packet_decoder_flac::Packet_decoder_flac() {}
Bytes Packet_decoder_flac::decode(const uint needed_sz) { return {}; }

struct Packet_decoder_opus::Packet_decoder_opus::Impl {};
Packet_decoder_opus::Packet_decoder_opus() {}
Bytes Packet_decoder_opus::decode(const uint needed_sz) { return {}; }

struct Packet_decoder_vorbis::Packet_decoder_vorbis::Impl {};
Packet_decoder_vorbis::Packet_decoder_vorbis() {}
Bytes Packet_decoder_vorbis::decode(const uint needed_sz) { return {}; }

struct Packet_decoder_raw::Packet_decoder_raw::Impl {};
Packet_decoder_raw::Packet_decoder_raw() {}
Bytes Packet_decoder_raw::decode(const uint needed_sz) { return {}; }
