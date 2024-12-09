#pragma once
#include "util/str.hpp"

namespace graphic {

inline constexpr std::string_view DEFAULT_PALETTE_FILES_DIR = "resource/image/palettes/";
inline constexpr std::string_view DEFAULT_PALETTE_FILE = "resource/image/palettes/default.png";
// текущий файл палитры, если пустой, то дефолтная генерация
inline Str current_palette_file {DEFAULT_PALETTE_FILE};

}
