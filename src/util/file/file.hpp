#pragma once
#include "util/math/num-types.hpp"
#include "game/util/resource.hpp"

using Bytes = Vector<byte>;

// показывает имена всех файлов в директории
Strs all_names_in_dir(Str dir);

struct File: public Resource {
  Bytes data {};

  File() = default;
  File(cr<Bytes> _data, cr<Str> _path);
};
