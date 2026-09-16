#pragma once
#include "game/util/store.hpp"
#include "engine/graphic/sprite/sprite.hpp"

namespace hpw {

// все спрайты тут
inline Store<Sprite> sprites {};

// если true, грузить спрайты только по запросу по имени
inline bool lazy_load_sprite = true;

} // hpw ns
