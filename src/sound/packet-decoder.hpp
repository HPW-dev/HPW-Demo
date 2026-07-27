#pragma once
#include "util/mem-types.hpp"
#include "util/file/file.hpp"

struct Audio;

// Читает аудио файл блоками
class Packet_decoder {
  nocopy(Packet_decoder);
public:
  explicit Packet_decoder() = default;
  virtual ~Packet_decoder() = default;
  virtual Bytes decode(const uint needed_sz) = 0;
};

#define MAKE_PACKET_DECODER(NAME) \
class NAME: public Packet_decoder { \
public: \
  explicit NAME(cr<Audio> sound); \
  ~NAME(); \
  Bytes decode(const uint needed_sz) override; \
 \
private: \
  nocopy(NAME); \
  struct Impl; \
  Unique<Impl> impl {}; \
};

MAKE_PACKET_DECODER(Packet_decoder_flac);
MAKE_PACKET_DECODER(Packet_decoder_opus);
MAKE_PACKET_DECODER(Packet_decoder_vorbis);
MAKE_PACKET_DECODER(Packet_decoder_raw);
