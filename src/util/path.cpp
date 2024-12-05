#include <filesystem>
#include <string>
#include "str-util.hpp"
#include "util/path.hpp"

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

void make_dir_if_not_exist(cr<Str> dir) {
  namespace fs = std::filesystem;
  try {
    // Check if src folder exists
    if ( !fs::is_directory(dir) || !fs::exists(dir))
      fs::create_directory(dir);
  } catch (cr<fs::filesystem_error> err) {
    // TODO надо правильно создать папку в рус-директории
  }
}

Str launch_dir_from_argv0(Cstr str) {
  Str path(str);

  // обрезать path до крайнего правого сепаратора
  std::size_t sep_pos = 0;
  cfor (i, path.size())
    if (path[i] == SEPARATOR)
      sep_pos = i;

  auto ret = path.substr(0, sep_pos);
  if (ret.empty())
    ret = '.';
  return ret + SEPARATOR;
} // launch_dir_from_argv0
