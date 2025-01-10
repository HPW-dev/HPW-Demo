
#pragma once
#include <cassert>
#include <functional>
#include <unordered_map>
#include "menu.hpp"
#include "util/str.hpp"

class Yaml;
class Menu;

template <class T>
struct is_std_function {
  constexpr static bool value = false;
};

template <class T>
struct is_std_function<std::function<T>> {
  constexpr static bool value = true;
};

template <class T>
constexpr static bool is_std_function_v = is_std_function<T>::value;

// сохраняет ссылку на фунцию и её тип
struct Action_container {
  std::uintptr_t action_address {};
  std::size_t action_type_hash {};

  template <class FUNC>
  inline explicit Action_container(cr<FUNC> f) {
    static_assert(is_std_function_v<FUNC>, "need std::function as args"); 
    assert(f);
    cauto f_addr = std::addressof(f);
    cauto f_cptr = rcast<cp<FUNC>>(f_addr);
    action_address = rcast<std::uintptr_t>(f_cptr);
    action_type_hash = typeid(FUNC).hash_code();
  }

  // проверить что типы функций совпадают
  template <class F>
  inline bool is_same_type() const {
    static_assert(is_std_function_v<F>, "need std::function as args");
    return action_type_hash == typeid(F).hash_code();
  }

  // каст с проверкой
  template <class F>
  inline cr<F> cast_to() const {
    assert(action_address != 0);
    cauto ret_cptr = rcast<cp<F>>(action_address);
    return rcast<cr<F>>(*ret_cptr);
  }
};

using Action_table = std::unordered_map<Str, Action_container>;

// создаёт текстовую менюшки из yaml-конфига
Unique<Menu> menu_from_yaml(cr<Yaml> config, cr<Action_table> actions);
