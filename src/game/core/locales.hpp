#pragma once
#include "game/util/store.hpp"
#include "util/str.hpp"

class Locale;

namespace hpw {
inline Shared<Store<Locale>> store_locale {}; // локализованные строки
inline const Str fallback_font_path = "resource/locale/EN (Google).yml";
inline Str locale_path = fallback_font_path; // текущая выбранная локаль
}
