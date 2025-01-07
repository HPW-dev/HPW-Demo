#pragma once
#include <atomic>
#include <utility>
#include "util/bytes.hpp"
#include "util/str.hpp"

namespace net {
  
struct Packet final {
  Str source_address {}; // от кого пришёл пакет
  Bytes bytes {}; // данные пакета
  std::atomic_bool loaded_correctly {}; // true когда успешно получен

  Packet() = default;
  ~Packet() = default;
  inline Packet(Packet&& other) {
    if (this != std::addressof(other)) {
      source_address = std::move(other.source_address);
      bytes = std::move(other.bytes);
      loaded_correctly = other.loaded_correctly.load();
    }
  }
}; // Packet

using Packets = Vector<Packet>;

constexpr static const std::size_t PACKET_BUFFER_SZ = 400;

} // net ns
