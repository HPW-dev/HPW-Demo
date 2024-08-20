#pragma once

#ifdef HOST_GLFW3
#include <GL/glew.h>
#else
using GLfloat = float;
#endif

#include "util/str.hpp"
#include "util/macro.hpp"
#include "util/vector-types.hpp"

struct Pal8;
struct Rgb24;

// меняет лимит обновлений игры
void set_target_ups(int new_ups);
// загружкает палитру для 1D текстуры в opengl
Vector<GLfloat> load_ogl_palette(cr<Str> fname);
// красит Pal8 -> RGB24 с помощью таблицы из файла
Rgb24 pal8_to_rgb24_by_file(const Pal8 x);
// случайное название для окна игры
Str rnd_window_name();
