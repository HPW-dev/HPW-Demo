#pragma once 
#include "packet.hpp"
#include "util/unicode.hpp"
#include "util/vector-types.hpp"

namespace net {

struct Player_info {
  utf32 nickname {};
  Str ip_v4 {};
  bool connected {};
};

// инфа о подключенных игроках
class Pck_connection_info: public Pck_gen_base {
public:
  utf32 self_nickname {};
  Vector<Player_info> players {};
  bool is_server {};

  void from_packet(cr<Packet> src) override;
  Packet to_packet(bool with_hash = true) const override;
  inline Tag tag() const override { return Tag::CONNECTION_INFO; }
};

} // net ns
