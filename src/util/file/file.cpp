#include <fstream>
#include <string>
#include <filesystem>
#include "file.hpp"
#include "util/error.hpp"
#include "util/str-util.hpp"

/// определяет размер файла в байтах
inline auto file_size(auto &file) {
  file.seekg(0, std::ios::end);
  auto size {file.tellg()};
  file.seekg(0, std::ios::beg);
  return size;
}

Bytes mem_from_file(Str fname) {
  iferror(fname.empty(), "name is empty");
  conv_sep(fname);
  std::ifstream file(fname, std::ios_base::binary);
  iferror( !file, "file \"" << fname << "\" not readed\n");
  Bytes mem(file_size(file));
  iferror( !(mem.size() > 0 && mem.size() != std::size_t(-1)),
    "bad file size");
  // RAW данные с файла
  file.read(ptr2ptr<char*>(mem.data()), mem.size());
  return mem;
} // mem_from_file

inline void all_names_in_dir_helper(Strs& v_name, CN<Str> dir) {
  for (cnauto entry: std::filesystem::directory_iterator(dir)) {
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

void mem_to_file(CN<Bytes> data, Str fname) {
  iferror(fname.empty(), "name is empty");
  conv_sep(fname);
  std::ofstream file(fname, std::ios_base::binary);
  iferror( !file, "file \"" << fname << "\" not opened for save\n");
  iferror( data.empty(), "saving data is empty");
  // RAW данные в файл
  file.write(cptr2ptr<CP<char>>(data.data()), data.size());
}
