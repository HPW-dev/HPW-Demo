#pragma once
#include "util/str.hpp"
#include "util/mem-types.hpp"
#include "util/macro.hpp"

class Yaml;

namespace epge {
class Base;
}

void save_epges(Yaml& config); // сохранить настройки epge
void load_epges(cr<Yaml> config); // загрузить настройки epge
Strs avaliable_epges(); // узнать какие эффекты есть на выбор
Unique<epge::Base> make_epge(cr<Str> name); // получить эффект по его имени
bool remove_epge(cp<epge::Base> address); // удалить конкретный эффект. True, если получилось
