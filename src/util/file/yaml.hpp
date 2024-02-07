#pragma once
#include "game/util/resource.hpp"
#include "util/unicode.hpp"
#include "util/mem-types.hpp"
#include "util/file/file.hpp"

/// парсер .yml файлов
class Yaml final: public Resource {
  class Impl;
  Unique<Impl> impl {};

public:
  /** создать или открыть .yml файл
  * @param fname имя файла на диске
  * @param make_if_not_exist создать, если файла нету */
  Yaml(Str fname, bool make_if_not_exist=false);
  /// загрузить .yml из памяти файла
  Yaml(CN<File> file);
  Yaml(CN<Impl> new_impl);
  Yaml();
  ~Yaml();
  Yaml(CN<Yaml> other);
  Yaml(Yaml&& other) noexcept;
  Yaml& operator = (CN<Yaml> other);
  Yaml& operator = (Yaml&& other) noexcept;

  /// сохранить .yml на диск
  void save(Str fname);
  /// стереть все данные
  void clear();
  /// создать пустую ветвь
  Yaml make_node(CN<Str> name);
  /// удалить ветку
  void delete_node(CN<Str> name);

  void set_int(CN<Str> name, int val);
  void set_real(CN<Str> name, real val);
  void set_bool(CN<Str> name, bool val);
  void set_str(CN<Str> name, CN<Str> val);
  void set_utf8(CN<Str> name, CN<utf8> val);
  void set_v_int(CN<Str> name, CN<Vector<int>> val);
  void set_v_str(CN<Str> name, CN<Vector<Str>> val);
  void set_v_real(CN<Str> name, CN<Vector<real>> val);

  /// get node by name
  Yaml operator[] (CN<Str> name) const;
  Str get_str(CN<Str> name, CN<Str> def={}) const;
  int get_int(CN<Str> name, int def={}) const;
  real get_real(CN<Str> name, real def={}) const;
  bool get_bool(CN<Str> name, bool def={}) const;
  utf8 get_utf8(CN<Str> name, CN<utf8> def={}) const;
  Vector<Str> get_v_str(CN<Str> name, CN<Vector<Str>> def={}) const;
  Vector<int> get_v_int(CN<Str> name, CN<Vector<int>> def={}) const;
  Vector<real> get_v_real(CN<Str> name, CN<Vector<real>> def={}) const;

  /// string key and string value
  struct kv_t { 
    Str key {};
    Str value {};
  };
  /// string key and utf32 value
  struct kvu32_t {
    Str key {};
    utf32 str {};
  };
  using vkv_t = Vector<kv_t>; /// vector of string keys and string values
  using vkvu32_t = Vector<kvu32_t>; /// vector of string keys and utf32 values

  /// вернёт список ключей и значений: {"tag1.tag2.tag3", "value"}
  vkv_t get_kv_table() const;
  /// вернёт список ключей и значений: {"tag1.tag2.tag3", U"value"}
  vkvu32_t get_kvu32_table() const;
  /// получить название тегов рута (все верхние ноды)
  Strs root_tags() const;
  /// проверяет валидность (не заменяй это на op-bool)
  bool check() const;
}; // Yaml
