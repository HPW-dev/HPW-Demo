#pragma once
#include "store.hpp"

class Locale;

namespace hpw {
inline Shared<Store<Locale>> store_locale {}; /// локализованные строки
}
