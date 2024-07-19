#pragma once
#include "game/util/store.hpp"
#include "graphic/sprite/sprite.hpp"

namespace hpw {

inline Unique<Store<Sprite>> store_sprite {}; // все спрайты тут

// если true, все спрайты не загрузятся, а только по запросу
inline bool lazy_load_sprite {
#ifdef DEBUG
  true
#else
  false
#endif
};

}
