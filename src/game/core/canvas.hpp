#pragma once
#include "engine/graphic/image/image.hpp"

// настройки графики
namespace graphic {
inline sconst int width  = 512;  // ширина canvas
inline sconst int height = 384;  // высота canvas
inline Image canvas {Image(width, height, Pal8::black)}; // буффер рендера
}
