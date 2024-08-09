#pragma once
#include <cassert>
#include <utility>
#include <algorithm>
#include <functional>
#include <unordered_map>
#include "util/str.hpp"
#include "util/log.hpp"
#include "util/macro.hpp"
#include "util/mem-types.hpp"

// хранилище контента
template <class T>
class Store {
public:
  using Velue = Shared<T>;
  using Find_err_cb = std::function<Velue (CN<Str> name)>;

  Store() = default;
  ~Store() = default;
  Velue& push(CN<Str> name, CN<Velue> res);
  Velue& move(CN<Str> name, Shared<T>&& res);
  Velue find(CN<Str> name) const;
  // узнать названия всех ресурсов
  Strs list(bool without_generated=false) const;
  void move_find_err_cb(Find_err_cb&& cb);

private:
  nocopy(Store);
  std::unordered_map<Str, Velue> m_table {};
  // если не удаётся найти ресурс, выполнить калбэк
  Find_err_cb m_find_err_cb {};
}; // Store

// ----------------------- impl ------------------------------

template <class T>
Store<T>::Velue Store<T>::find(CN<Str> name) const {
  try {
    return m_table.at(name);
  } catch (...) {
    return_if (m_find_err_cb, m_find_err_cb(name));
    detailed_iflog( !name.empty(), "resource \"" << name << "\" not finded\n" );
  }
  return {};
}

template <class T>
Store<T>::Velue& Store<T>::push(CN<Str> name, CN<Velue> res) {
  detailed_log("Store.push: " << name << "\n");
  res->set_path(name);
  if (m_table.count(name) != 0)
    detailed_log("Store.push: reinit resource "
      "(это может стать причиной ошибки access free-object error)\n");
  m_table[name] = res;
  return m_table.at(name);
}

template <class T>
Store<T>::Velue& Store<T>::move(CN<Str> name, Velue&& res) {
  detailed_log("Store.move: " << name << "\n");
  res->set_path(name);
  if (m_table.count(name) != 0)
    detailed_log("Store.move: reinit resource "
      "(это может стать причиной ошибки access free-object error)\n");
  m_table[name] = std::move(res);
  return m_table.at(name);
}

template <class T>
Strs Store<T>::list(bool without_generated) const {
  Strs list;
  for (cnauto [name, res]: m_table) {
    Str out_name;
    cont_if (without_generated && res && res->is_generated());
    list.emplace_back(name);
  } 
  std::sort(list.begin(), list.end());
  return list;
}

template <class T>
void Store<T>::move_find_err_cb(Find_err_cb&& cb) {
  assert(cb);
  m_find_err_cb = std::move(cb);
}
