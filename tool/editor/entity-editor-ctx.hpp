#pragma once
#include "util/mem-types.hpp"
#include "util/file/yaml.hpp"

// для обмена данными между окнами редактора игровых объектов
struct Entity_editor_ctx {
  bool pause {};
  bool red_bg {true};
  float bg_color {0.333333};
  Yaml entities_yml {};
};
