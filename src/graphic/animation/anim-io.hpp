#pragma once
#include "util/macro.hpp"

class Yaml;

// загрузить все анимации в yml файл
void read_anims(CN<Yaml> src);
// сохраняет все анимации в yml файл
void save_anims(Yaml& dst);
