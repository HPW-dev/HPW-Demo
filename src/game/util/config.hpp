#pragma once
#include <thirdparty/include/nlohmann/json_fwd.hpp>
#include "util/mem-types.hpp"

void save_config();
void load_config();
void load_config_input(const nlohmann::json& config); // загрузить только настройки управления
void load_config_graphic(const nlohmann::json& config); // загрузить только настройки графики
void load_config_game(const nlohmann::json& config); // загрузить только настройки игры

namespace hpw {
inline nlohmann::json config {}; // config.json
}
