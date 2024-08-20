#pragma once
#include "util/unicode.hpp"
#include "game/util/resource.hpp"

class Yaml;

// менеджер локализованных строк
class Locale final: public Resource {
public:
  utf32 str {};
  
  inline Locale(cr<utf32> in): str{in} {}
  Locale() = default;
  ~Locale() = default;
};

void load_locales_to_store(Yaml file);
