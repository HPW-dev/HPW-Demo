#pragma once
#include "util/mem-types.hpp"
#include "engine/graphic/image/image.hpp"

// настройки графики
namespace graphic {
inline sconst int width  = 512;  // ширина canvas
inline sconst int height = 384;  // высота canvas
inline Unique<Image> canvas {}; // буффер рендера
}
