#pragma once
#include "util/math/vec.hpp"
#include "engine/graphic/image/image-fwd.hpp"

// Заливает все соседние пиксели цветом <filler> с позиции <pos>
void fill(Image& dst, Vec pos, Pal8 filler);
