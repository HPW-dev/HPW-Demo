#pragma once
#include "util/math/num-types.hpp"
#include "game/util/resource.hpp"

using Bytes = Vector<byte>;

// получает сырые данные из файла
Bytes mem_from_file(Str fname);

void mem_to_file(CN<Bytes> data, Str fname);

// показывает имена всех файлов в директории
Strs all_names_in_dir(Str dir);

struct File: public Resource {
  Bytes data {};

  File() = default;
  File(CN<Bytes> _data, CN<Str> _path);
};
