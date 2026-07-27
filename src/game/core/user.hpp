#pragma once
#include <cstdint>
#include "util/unicode.hpp"
#include "util/str.hpp"

namespace hpw {
inline constexpr std::uint32_t MAX_NICKNAME_SZ = 1024 * 1024 * 300; // лимит на размер никнейма игрока
inline utf32 player_name {}; // текущее имя игрока
inline bool text_input_mode {}; // когда true, записывать текст с клавы в text_input
inline utf32 text_input {}; // строка введённая через клавиатуру
inline int text_input_pos {}; // какой сейчас символ текста редачится
inline utf32 user_warnings {}; // все строки отсюда будут показаны в специальном окне предупреждения
inline Str default_tile {}; // название окна игры. Если не задано, то будет случайным
}
