#pragma once
#include "util/mem-types.hpp"
#include "graphic/image/image.hpp"

// настройки графики
namespace graphic {
inline int width  = 512;  // ширина canvas
inline int height = 384;  // высота canvas
inline Unique<Image> canvas {}; // буффер рендера
}
