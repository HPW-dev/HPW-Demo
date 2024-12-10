#pragma once
#include "util/str.hpp"
#include "util/mem-types.hpp"
#include "util/macro.hpp"

class Yaml;

namespace epge {
class Base;
}

// сохранить настройки epge
void save_epges(Yaml& config);
// загрузить настройки epge
void load_epges(cr<Yaml> config);
// узнать какие эффекты есть на выбор
Strs avaliable_epges();
// получить эффект по его имени
Unique<epge::Base> make_epge(cr<Str> name);
