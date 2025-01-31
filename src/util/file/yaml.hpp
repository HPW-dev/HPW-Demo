#pragma once
#include "util/unicode.hpp"
#include "util/mem-types.hpp"
#include "util/math/num-types.hpp"
#include "game/util/resource.hpp"

struct File;

// парсер .yml файлов
class Yaml final: public Resource {
  struct Impl;
  Unique<Impl> impl {};

public:
  Yaml() noexcept;
  ~Yaml();
  
  /** создать или открыть .yml файл
  * @param fname имя файла на диске
  * @param make_if_not_exist создать, если файла нету */
  Yaml(Str fname, bool make_if_not_exist=false);
  Yaml(cr<File> file); // загрузить .yml из памяти файла
  Yaml(cr<Impl> new_impl);
  Yaml(cr<Yaml> other);
  Yaml(Yaml&& other) noexcept;
  Yaml& operator = (cr<Yaml> other) noexcept;
  Yaml& operator = (Yaml&& other) noexcept;

  void save(Str fname) const; // сохранить .yml на диск
  void clear(); // стереть все данные
  Yaml make_node(cr<Str> name); // создать пустую ветвь
  // не создаст пустую ветвь, если она уже есть с таким именем
  Yaml make_node_if_not_exist(cr<Str> name);
  void delete_node(cr<Str> name); // удалить ветку

  void set_int(cr<Str> name, int val);
  void set_real(cr<Str> name, real val);
  void set_bool(cr<Str> name, bool val);
  void set_str(cr<Str> name, cr<Str> val);
  void set_utf8(cr<Str> name, cr<utf8> val);
  void set_v_int(cr<Str> name, cr<Vector<int>> val);
  void set_v_str(cr<Str> name, cr<Vector<Str>> val);
  void set_v_real(cr<Str> name, cr<Vector<real>> val);

  // get node by name
  Yaml operator[] (cr<Str> name) const;
  Str get_str(cr<Str> name, cr<Str> def={}) const;
  int get_int(cr<Str> name, int def={}) const;
  real get_real(cr<Str> name, real def={}) const;
  bool get_bool(cr<Str> name, bool def={}) const;
  utf8 get_utf8(cr<Str> name, cr<utf8> def={}) const;
  Vector<Str> get_v_str(cr<Str> name, cr<Vector<Str>> def={}) const;
  Vector<int> get_v_int(cr<Str> name, cr<Vector<int>> def={}) const;
  Vector<real> get_v_real(cr<Str> name, cr<Vector<real>> def={}) const;

  // string key and string value
  struct kv_t { 
    Str key {};
    Str value {};
  };
  // string key and utf32 value
  struct kvu32_t {
    Str key {};
    utf32 str {};
  };
  using vkv_t = Vector<kv_t>; // vector of string keys and string values
  using vkvu32_t = Vector<kvu32_t>; // vector of string keys and utf32 values

  vkv_t get_kv_table() const; // вернёт список ключей и значений: {"tag1.tag2.tag3", "value"}
  vkvu32_t get_kvu32_table() const; // вернёт список ключей и значений: {"tag1.tag2.tag3", U"value"}
  Strs root_tags() const; // получить название тегов рута (все верхние ноды)
  bool check() const; // проверяет валидность (не заменяй это на op-bool)
  Vector<Yaml> items() const; // пройтись по всем вложенным нодам в текущей
}; // Yaml
