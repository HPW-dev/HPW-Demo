#pragma once
#include "util/mem-types.hpp"
#include "util/net/net.hpp"

class Image;

// рулит сетевым взаимодействием в этом тесте
class Netcode final {
public:
  explicit Netcode(bool is_server, cr<Str> ip_v4="0", const net::Port port=net::DEFAULT_PORT);
  ~Netcode();
  // если есть широковещательное приглашение, то законектиться к нему
  void connect_to_broadcast();
  // зайти по айпишнику
  void connect_to(cr<Str> ip_v4);
  void draw(Image& dst) const;
  void update(const Delta_time dt);

private:
  struct Impl;
  Unique<Impl> _impl {};
};
