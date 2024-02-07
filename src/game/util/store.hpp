#pragma once
#include <algorithm>
#include <unordered_map>
#include "util/macro.hpp"
#include "util/str.hpp"
#include "util/mem-types.hpp"
#include "util/log.hpp"

/// хранилище контента
template <class T>
class Store {
public:
  Store() = default;
  ~Store() = default;
  Shared<T>& push(CN<Str> name, CN<Shared<T>> res);
  CN<Shared<T>> find(CN<Str> name) const;
  /// узнать названия всех ресурсов
  Strs list(bool without_generated=false) const;

private:
  std::unordered_map<Str, Shared<T>> table {};
}; // Store

// ----------------------- impl ------------------------------

template <class T>
CN<Shared<T>> Store<T>::find(CN<Str> name) const {
  try {
    return table.at(name);
  } catch (...) {
    detailed_iflog( !name.empty(),
      "resource \"" << name << "\" not finded\n" );
  }
  static Shared<T> null_res {};
  return null_res;
}

template <class T>
Shared<T>& Store<T>::push(CN<Str> name, CN<Shared<T>> res) {
  detailed_log("Store.push: " << name << "\n");
  res->set_path(name);
  if (table.count(name) != 0)
    detailed_log("Store.push: reinit resource (это может стать причиной ошибки access free-object error)\n");
  table[name] = res;
  return table.at(name);
}

template <class T>
Strs Store<T>::list(bool without_generated) const {
  Strs list;
  for (cnauto [name, res]: table) {
    Str out_name;
    cont_if (without_generated && res && res->is_generated());
    list.emplace_back(name);
  } 
  std::sort(list.begin(), list.end());
  return list;
}
