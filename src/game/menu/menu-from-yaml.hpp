
#pragma once
#include <cassert>
#include <unordered_map>
#include "menu.hpp"
#include "util/str.hpp"

class Yaml;
class Menu;

// сохраняет ссылку на фунцию и её тип
struct Action_container {
  std::uintptr_t action_address {};
  std::size_t action_type_hash {};

  template <class F>
  inline explicit Action_container(F* f) {
    assert(f != nullptr);
    action_address = rcast<std::uintptr_t>(f);
    action_type_hash = typeid(F).hash_code();
  }

  // проверить что типы функций совпадают
  template <class F>
  inline bool is_same_type() const { return action_type_hash == typeid(F).hash_code(); }

  // каст с проверкой
  template <class F>
  inline const F* cast_to() const {
    assert(action_address != 0);
    assert(is_same_type<F>());
    return rcast<const F*>(action_address);
  }
};

using Action_table = std::unordered_map<Str, Action_container>;

// создаёт текстовую менюшки из yaml-конфига
Unique<Menu> menu_from_yaml(cr<Yaml> config, cr<Action_table> actions);
