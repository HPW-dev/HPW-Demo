#include <locale>
#include <codecvt>
#include "unicode.hpp"

utf32 utf8_to_32(CN<Str> text) {
  using utf8_to_utf32 = std::codecvt_utf8<char32_t>;
  std::wstring_convert<utf8_to_utf32, char32_t> conv;
  return conv.from_bytes(text);
}
