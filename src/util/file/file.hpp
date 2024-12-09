#pragma once
#include "util/math/num-types.hpp"
#include "game/util/resource.hpp"

using Bytes = Vector<byte>;

// получает сырые данные из файла
Bytes mem_from_file(Str fname);
void mem_to_file(cr<Bytes> data, Str fname);
// показывает имена всех файлов в директории
Strs all_names_in_dir(Str dir);

struct File: public Resource {
  Bytes data {};

  File() = default;
  File(cr<Bytes> _data, cr<Str> _path);
};
