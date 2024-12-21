#pragma once
#include "game/util/store.hpp"
#include "util/str.hpp"

class Locale;

namespace hpw {
inline Shared<Store<Locale>> store_locale {}; // локализованные строки
inline const Str fallback_locale_path = "resource/locale/RU (Mramor).yml";
inline Str locale_path = fallback_locale_path; // текущая выбранная локаль
}
