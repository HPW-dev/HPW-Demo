#pragma once
#include <unordered_map>
#include <functional>
#include "util/str.hpp"
#include "util/mem-types.hpp"
#include "util/macro.hpp"

class Yaml;

namespace epge {

class Base;
using Maker = std::function< Unique<epge::Base>() >;
using Makers = std::unordered_map<Str, Maker>;
Makers& get_makers();

} // epge ns

template <class T>
inline void add_epge() {
  cauto epge = new_unique<T>();
  cauto name = epge->name();
  epge::get_makers()[name] = []{ return new_unique<T>(); };
}

void save_epges(Yaml& config); // сохранить настройки epge
void load_epges(cr<Yaml> config); // загрузить настройки epge
Strs avaliable_epges(); // узнать какие эффекты есть на выбор
Unique<epge::Base> make_epge(cr<Str> name); // получить эффект по его имени
bool remove_epge(cp<epge::Base> address); // удалить конкретный эффект. True, если получилось
