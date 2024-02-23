#pragma once
#include "util/mem-types.hpp"

class Image;

/// настройки графики
namespace graphic {
inline int width  = 512;  /// ширина canvas
inline int height = 384;  /// высота canvas
inline Shared<Image> canvas {}; /// буффер рендера
}
