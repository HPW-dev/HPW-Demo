#pragma once
#include "game/util/store.hpp"
#include "graphic/sprite/sprite.hpp"

namespace hpw {

// все спрайты тут
inline Unique<Store<Sprite>> store_sprite {};

// если true, грузить спрайты только по запросу по имени
inline bool lazy_load_sprite {
#ifdef DEBUG
  true
#else
  false
#endif
};

} // hpw ns
