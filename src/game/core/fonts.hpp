#pragma once
#include "graphic/font/font.hpp"
#include "util/mem-types.hpp"

namespace graphic {
inline Unique<hpw::Font> font; // обычный шрифт игры
inline Unique<hpw::Font> font_shop; // шрифт для надписей в магазине
inline Unique<hpw::Font> system_mono; // моноширинный шрифт для системной информации
}
