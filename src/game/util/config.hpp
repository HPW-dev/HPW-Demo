#pragma once
#include "util/mem-types.hpp"

class Yaml;

void save_config();
void load_config();

namespace hpw {
inline Shared<Yaml> config {}; // config.yml
}
