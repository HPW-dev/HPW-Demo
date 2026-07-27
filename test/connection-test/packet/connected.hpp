#pragma once 
#include "packet.hpp"
#include "util/unicode.hpp"
#include "util/vector-types.hpp"

namespace net {
// инфа подтверждение об успешном подключении
class Pck_connected: public Pck_gen_base {
public:
  void from_packet(cr<Packet> src) override;
  Packet to_packet(bool with_hash = true) const override;
  inline Tag tag() const override { return Tag::CONNECTED; }
};

} // net ns
