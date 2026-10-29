#include "pch.hpp"
#include "sprite-helper.hpp"

#include "game/core/sprites.hpp"

cr<Image> find_image_safe(cr<Str> name) {
  auto spr = hpw::sprites.find(name);
  iferror(!spr, "sprite \"" << name << "\" not finded");

  return spr->image();
}
