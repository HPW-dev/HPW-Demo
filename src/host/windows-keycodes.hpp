#pragma once
#include <optional>
#include "util/unicode.hpp"

std::optional<utf32> get_windows_keycode_name(int keycode);
