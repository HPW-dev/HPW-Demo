#pragma once
#include "util/macro.hpp"
#include "util/file/yaml.hpp"
#include "util/mem-types.hpp"

class Anim;

// загрузить все анимации из yml файла
void read_anims(cr<Yaml> src);
// читает только одну анимацию из ноды конфига
Shared<Anim> read_anim(cr<Yaml> anim_node);
// сохраняет все анимации в yml файл
void save_anims(Yaml& dst);
// получить файл конфига с анимациями
Yaml get_anim_config();
