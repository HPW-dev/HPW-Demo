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
  // элементы хранятся в смартпоинтерах
  using Velue = Shared<T>;
  // действие при отсутствии искомого элемента
  using Find_err_cb = std::function<Velue (cr<Str> name)>;

  Store() = default;
  ~Store() = default;
  // зарегистрировать элемент res по имени name
  Velue& push(cr<Str> name, cr<Velue> res);
  // зарегистрировать элемент res по имени name (с перемещением)
  Velue& move(cr<Str> name, Shared<T>&& res);
  // найти элемент с именем name. При ошибке вызвать функцию из move_find_err_cb
  Velue find(cr<Str> name) const;
  // найти элемент с именем name. При ошибке вызвать функцию из move_find_err_cb
  Velue operator[](cr<Str> name) const;
  // узнать названия всех ресурсов
  Strs list(bool without_generated=false) const;
  // добавить лямбду при ошибках поиска элемента
  void move_find_err_cb(Find_err_cb&& cb);
  // удалить все элементы
  void clear();
  // проверить что есть элементы
  bool empty() const;

private:
  nocopy(Store);
  std::unordered_map<Str, Velue> m_table {}; // имя-значение
  // если не удаётся найти ресурс, выполнить калбэк
  Find_err_cb m_find_err_cb {};
};

// ----------------------- impl ------------------------------

template <class T>
Store<T>::Velue Store<T>::find(cr<Str> name) const {
  try {
    return m_table.at(name);
  } catch (...) {
    return_if (m_find_err_cb, m_find_err_cb(name));
    if (!name.empty())
      log_error << "resource \"" + name + "\" not finded";
  }
  return {};
}

template <class T>
Store<T>::Velue Store<T>::operator[](cr<Str> name) const {
  return this->find(name);
}

template <class T>
Store<T>::Velue& Store<T>::push(cr<Str> name, cr<Velue> res) {
  log_debug << "Store.push: " + name;
  res->set_path(name);
  if (m_table.count(name) != 0)
    log_warning << 
      "Store.push: reinit resource "
      "(это может стать причиной ошибки access free-object error)";
  m_table[name] = res;
  return m_table.at(name);
}

template <class T>
Store<T>::Velue& Store<T>::move(cr<Str> name, Velue&& res) {
  log_debug << "Store.move: " + name;
  res->set_path(name);
  if (m_table.count(name) != 0)
    log_warning << 
      "Store.move: reinit resource "
      "(это может стать причиной ошибки access free-object error)";
  m_table[name] = std::move(res);
  return m_table.at(name);
}

template <class T>
Strs Store<T>::list(bool without_generated) const {
  Strs list;
  for (crauto [name, res]: m_table) {
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

template <class T>
void Store<T>::clear() {
  m_table.clear();
  m_find_err_cb = {};
}

template <class T>
bool Store<T>::empty() const { return m_table.empty(); }
