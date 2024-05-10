#pragma once
#include "util/mem-types.hpp"
#include "util/file/file.hpp"

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
  nocopy(NAME); \
  struct Impl; \
  Unique<Impl> impl {}; \
public: \
  explicit NAME(); \
  ~NAME(); \
  Bytes decode(const uint needed_sz) override; \
};

MAKE_PACKET_DECODER(Packet_decoder_flac);
MAKE_PACKET_DECODER(Packet_decoder_opus);
MAKE_PACKET_DECODER(Packet_decoder_vorbis);
MAKE_PACKET_DECODER(Packet_decoder_raw);
