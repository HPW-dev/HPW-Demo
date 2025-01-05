#pragma once
#include "graphic/font/font.hpp"
#include "util/mem-types.hpp"

namespace graphic {
inline Unique<Font_base> font; // обычный шрифт игры
inline Unique<Font_base> font_shop; // шрифт для надписей в магазине
inline Unique<Font_base> system_mono; // моноширинный шрифт для системной информации
inline Unique<Font_base> asci_3x5; // моноширинный маленький пиксельный шрифт с латинскими буквами
inline Unique<Font_base> asci_5x5; // моноширинный маленький пиксельный шрифт с латинскими буквами
}
