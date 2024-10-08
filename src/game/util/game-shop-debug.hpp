#pragma once
#include "util/macro.hpp"
#include "util/mem-types.hpp"
#include "util/math/num-types.hpp"

class Image;

// дебажное меню выбора всех способностей
class Shop_debug {
  nocopy(Shop_debug);
  struct Impl;
  Unique<Impl> impl {};

public:
  Shop_debug();
  ~Shop_debug();
  void draw(Image& dst) const;
  // @return false - выбор в меню сделан, его можно отключить
  bool update(const Delta_time dt);
}; // Shop_debug

