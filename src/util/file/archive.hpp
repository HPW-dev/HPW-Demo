#pragma once
#include "game/util/resource.hpp"
#include "util/macro.hpp"
#include "util/str-util.hpp"
#include "util/file/file.hpp"

struct zip_t;

// для работы с архива типа .zip
class Archive final: public Resource {
  zip_t *zip {};
  Bytes strm_buf {};

  void _find_name_rec(cr<Str> fname, Strs& dst) const;
  void _zip_check(int errcode, cr<Str> fname) const;

public:
  explicit Archive(Str fname);
  ~Archive();
  // получить файл из архива в виде RAW данных
  File get_file(Str fname) const;
  // все имена файлов в архиве
  Strs get_all_names(bool with_folders=true) const;
}; // Archive
