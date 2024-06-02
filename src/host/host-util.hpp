#pragma once
#include <GL/glew.h>
#include "util/str.hpp"
#include "util/macro.hpp"
#include "util/vector-types.hpp"

struct Pal8;
struct Rgb24;

// меняет лимит обновлений игры
void set_target_ups(int new_ups);
// загружкает палитру для 1D текстуры в opengl
Vector<GLfloat> load_ogl_palette(CN<Str> fname);
// преобразует палитровый цвет в RGB24 с учётом текущей палитры
Rgb24 to_palette_rgb24(const Pal8 x);
