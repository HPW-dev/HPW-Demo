#include <filesystem>
#include <string>
#include "str-util.hpp"
#include "util/path.hpp"

Str get_filename(CN<Str> str) {
  std::filesystem::path path(str);
  return path.stem().string();
}

Str get_fileext(CN<Str> str) {
  std::filesystem::path path(str);
  return path.extension().string();
}

Str get_filedir(CN<Str> str)
  { return str.substr(0, str.find_last_of(SEPARATOR)); }

std::list<Str> files_in_dir(CN<Str> path) {
  std::list<Str> ret = {};
  for (cnauto entry : std::filesystem::directory_iterator(path))
    ret.push_back(entry.path().string());
  return ret;
}

void make_dir_if_not_exist(CN<Str> dir) {
  namespace fs = std::filesystem;
  try {
    // Check if src folder exists
    if ( !fs::is_directory(dir) || !fs::exists(dir))
      fs::create_directory(dir);
  } catch (CN<fs::filesystem_error> err) {
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
