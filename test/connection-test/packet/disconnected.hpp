#pragma once 
#include "packet.hpp"
#include "util/unicode.hpp"
#include "util/vector-types.hpp"

namespace net {
// инфа подтверждение об успешном подключении
class Pck_disconnected: public Pck_gen_base {
public:
  bool disconnect_you {}; // если True, то это тебя отключили

  void from_packet(cr<Packet> src) override;
  Packet to_packet(bool with_hash = true) const override;
  inline Tag tag() const override { return Tag::DISCONNECTED; }
};

} // net ns
