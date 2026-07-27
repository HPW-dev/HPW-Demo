#pragma once
#include <utility>
#include "util/bytes.hpp"
#include "game/util/resource.hpp"

// показывает имена всех файлов в директории
Strs all_names_in_dir(Str dir);

struct File: public Resource {
  Bytes data {};

  File() = default;
  inline File(cr<Bytes> _data, cr<Str> _path): Resource(_path), data {_data} {}
  inline File(Bytes&& _data, cr<Str> _path): Resource(_path), data {std::move(_data)} {}
};
