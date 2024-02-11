#pragma once
#include "util/macro.hpp"
#include "util/str.hpp"

/// даёт рандомное ASCII лого
Str get_random_logo();
/// получить всю таблицу с ASCII логотипами
CN<Strs> get_all_logos();
