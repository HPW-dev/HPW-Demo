#include "pch.hpp"
#include "std-stream.hpp"

#ifdef WINDOWS
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
#endif


#ifdef WINDOWS
static std::wstring utf8_to_wstring(cr<Str> utf8) {
  ret_if (utf8.empty(), {});
  const int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8.data(),
    scast<int>(utf8.size()), nullptr, 0);
  ret_if (wlen <= 0, {});
  std::wstring wstr(wlen, L'\0');
  MultiByteToWideChar(CP_UTF8, 0, utf8.data(), scast<int>(utf8.size()),
    wstr.data(), wlen);
  return wstr;
}

std::ifstream open_ifstream(cr<Str> path, std::ios::openmode mode) {
  return std::ifstream(utf8_to_wstring(path).c_str(), mode);
}

std::ofstream open_ofstream(cr<Str> path, std::ios::openmode mode) {
  return std::ofstream(utf8_to_wstring(path).c_str(), mode);
}

#else // ---- Linux ------------------------------------------------------------
std::ifstream open_ifstream(cr<Str> path, std::ios::openmode mode) {
  return std::ifstream(path, mode);
}

std::ofstream open_ofstream(cr<Str> path, std::ios::openmode mode) {
  return std::ofstream(path, mode);
}
#endif
