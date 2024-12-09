#pragma once
#include "util/mem-types.hpp"

class Yaml;

void save_config();
void load_config();
void load_config_input(const Yaml& config); // загрузить только настройки управления
void load_config_graphic(const Yaml& config); // загрузить только настройки графики
void load_config_game(const Yaml& config); // загрузить только настройки игры

namespace hpw {
inline Shared<Yaml> config {}; // config.yml
}
