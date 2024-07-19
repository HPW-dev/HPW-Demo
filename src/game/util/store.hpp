#pragma once
#include <algorithm>
#include <functional>
#include <unordered_map>
#include "util/macro.hpp"
#include "util/str.hpp"
#include "util/mem-types.hpp"
#include "util/log.hpp"

// хранилище контента
template <class T>
class Store {
public:
  using Find_err_cb = std::function<Shared<T> (CN<Str> name)>;

  Store() = default;
  ~Store() = default;
  Shared<T>& push(CN<Str> name, CN<Shared<T>> res);
  Shared<T>& move(CN<Str> name, Shared<T>&& res);
  Shared<T> find(CN<Str> name) const;
  // узнать названия всех ресурсов
  Strs list(bool without_generated=false) const;
  void move_find_err_cb(Find_err_cb&& cb);

private:
  nocopy(Store);
  std::unordered_map<Str, Shared<T>> m_table {};
  // если не удаётся найти ресурс, выполнить калбэк
  Find_err_cb m_find_err_cb {};
}; // Store

// ----------------------- impl ------------------------------

template <class T>
Shared<T> Store<T>::find(CN<Str> name) const {
  try {
    return m_table.at(name);
  } catch (...) {
    detailed_iflog( !name.empty(),
      "resource \"" << name << "\" not finded\n" );
    if (m_find_err_cb)
      return m_find_err_cb(name);
  }
  return {};
}

template <class T>
Shared<T>& Store<T>::push(CN<Str> name, CN<Shared<T>> res) {
  detailed_log("Store.push: " << name << "\n");
  res->set_path(name);
  if (m_table.count(name) != 0)
    detailed_log("Store.push: reinit resource "
      "(это может стать причиной ошибки access free-object error)\n");
  m_table[name] = res;
  return m_table.at(name);
}

template <class T>
Shared<T>& Store<T>::move(CN<Str> name, Shared<T>&& res) {
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
  iferror(!cb, "cb is null");
  m_find_err_cb = std::move(cb);
}
