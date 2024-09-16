#pragma once
#include "util/unicode.hpp"

namespace hpw {
inline utf32 player_name {}; // текущее имя игрока
inline bool text_input_mode {}; // когда true, записывать текст с клавы в text_input
inline utf32 text_input {}; // строка введённая через клавиатуру
inline int text_input_pos {}; // какой сейчас символ текста редачится
inline utf32 user_warnings {}; // все строки отсюда будут показаны в специальном окне предупреждения
}
