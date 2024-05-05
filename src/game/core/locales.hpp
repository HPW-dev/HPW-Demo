#pragma once
#include "game/util/store.hpp"

class Locale;

namespace hpw {
inline Shared<Store<Locale>> store_locale {}; // локализованные строки
}
