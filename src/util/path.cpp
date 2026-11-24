#include "pch.hpp"

Str get_filename(cr<Str> str) {
  std::filesystem::path path(str);
  return path.stem().string();
}

Str get_fileext(cr<Str> str) {
  std::filesystem::path path(str);
  return path.extension().string();
}

Str get_filedir(cr<Str> str)
  { return str.substr(0, str.find_last_of(SEPARATOR)); }

Strs files_in_dir(cr<Str> path, const bool recusive) {
  Strs ret = {};
  for (crauto entry : std::filesystem::directory_iterator(path)) {
    cauto path = entry.path().string();
    ret.push_back(path);
    
    if (recusive && std::filesystem::is_directory(path)) {
      cauto addition = files_in_dir(path, recusive);
      for (crauto addition_path: addition)
        ret.push_back(addition_path);
    }
  }
  return ret;
}

void make_dir_if_not_exist(cr<Str> _dir) {
  namespace fs = std::filesystem;
  Str dir {_dir};
  conv_sep(dir);
  cauto path = std::filesystem::path(dir);

  if (!fs::is_directory(path) || !fs::exists(path))
    fs::create_directory(path);
}

[[nodiscard]] bool file_exists(cr<Str> fname) {
  namespace fs = std::filesystem;
  cauto path = std::filesystem::path(fname);
  
  try {
    return fs::exists(path);
  } catch (...) {
    log_warning << "не удалось определить, существует ли файл \"" + fname + "\"";
  }

  return false;
}
