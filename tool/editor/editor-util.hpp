#pragma once
#include "util/str-util.hpp"
#include "util/unicode.hpp"

// макрос для конвертирования строк для imGui
#define u8tos(str) sconv<Str>(utf8(str)).c_str()
