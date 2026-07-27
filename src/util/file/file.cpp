#include <string>
#include <filesystem>
#include "file.hpp"
#include "util/error.hpp"
#include "util/str-util.hpp"

inline void all_names_in_dir_helper(Strs& v_name, cr<Str> dir) {
  for (crauto entry: std::filesystem::directory_iterator(dir)) {
    auto path = entry.path().string();
    v_name.emplace_back(path);
    if (std::filesystem::is_directory(path))
      all_names_in_dir_helper(v_name, path);
  }
}

Strs all_names_in_dir(Str dir) {
  conv_sep(dir);
  Strs ret;
  all_names_in_dir_helper(ret, dir);
  return ret;
}
