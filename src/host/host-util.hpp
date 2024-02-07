#pragma once
#include "util/str.hpp"
#include "util/macro.hpp"
#include "util/vector-types.hpp"

/// меняет лимит обновлений игры
void set_target_ups(int new_ups);
/// чтобы показать что приложение запустилось
void print_logo();
/// загружкает палитру для 1D текстуры в opengl
Vector<float> load_ogl_palette(CN<Str> fname);
