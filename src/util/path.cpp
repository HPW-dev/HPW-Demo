#include <cassert>
#include <filesystem>
#include <string>
#include "str-util.hpp"
#include "log.hpp"
#include "unicode.hpp"
#include "platform.hpp"

#ifdef WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

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

#ifdef WINDOWS
// для конвертирования виндушных кодовых страниц в utf8 строку
static inline Str cp_acp_to_utf8(Cstr src) {
  const int result_u = ::MultiByteToWideChar(CP_ACP, 0, src, -1, 0, 0);
  if (result_u != 0) {
    Vector<WCHAR> ures(result_u, L'\0');
    if (::MultiByteToWideChar(CP_ACP, 0, src, -1, ures.data(), result_u)) {
      const int result_c = ::WideCharToMultiByte(CP_UTF8, 0, ures.data(), -1, 0, 0, 0, 0);
      if (result_c != 0) {
        Vector<char> cres(result_c, '\0');
        if (::WideCharToMultiByte(CP_UTF8, 0, ures.data(), -1, cres.data(), result_c, 0, 0))
          return Str(cres.data());
      }
    }
  }

  hpw_log("не удалось преобразовать строку \"" + Str(src) + "\"\n", Log_stream::warning);
  return Str(src);
}
#endif // WINDOWS

Str launch_dir_from_argv0(char** argv) {
  return_if (!argv || !argv[0], Str(".") + SEPARATOR);
  
  #ifdef WINDOWS
    Str path = cp_acp_to_utf8(argv[0]);
  #else
    Str path(argv[0]);
  #endif


  // обрезать path до крайнего правого сепаратора
  std::size_t sep_pos = 0;
  cfor (i, path.size())
    if (path[i] == SEPARATOR)
      sep_pos = i;

  auto ret = path.substr(0, sep_pos);
  if (ret.empty())
    ret = '.';
  return ret + SEPARATOR;
}

[[nodiscard]] bool file_exists(cr<Str> fname) {
  namespace fs = std::filesystem;
  cauto path = std::filesystem::path(fname);
  
  try {
    return fs::exists(path);
  } catch (...) {
    hpw_log("не удалось определить, существует ли файл \"" + fname + "\"\n", Log_stream::warning);
  }

  return false;
}
