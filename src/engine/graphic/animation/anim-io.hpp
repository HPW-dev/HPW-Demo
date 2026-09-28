#pragma once
#include <thirdparty/include/nlohmann/json_fwd.hpp>
#include "util/macro.hpp"
#include "util/mem-types.hpp"

class Anim;

// загрузить все анимации из yml файла
void read_anims(cr<nlohmann::json> src);
// читает только одну анимацию из ноды конфига
Shared<Anim> read_anim(cr<nlohmann::json> anim_node);
// сохраняет все анимации в yml файл
void save_anims(nlohmann::json& dst);
// получить файл конфига с анимациями
nlohmann::json get_anim_config();
