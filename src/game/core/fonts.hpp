#pragma once
#include "graphic/font/font.hpp"
#include "util/mem-types.hpp"

namespace graphic {
inline Unique<Font_base> font; // обычный шрифт игры
inline Unique<Font_base> font_shop; // шрифт для надписей в магазине
inline Unique<Font_base> system_mono; // моноширинный шрифт для системной информации
}
