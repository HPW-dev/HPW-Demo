#include <locale>
#include <codecvt>
#include "unicode.hpp"

utf32 utf8_to_32(cr<Str> text) {
  using utf8_to_utf32 = std::codecvt_utf8<char32_t>;
  std::wstring_convert<utf8_to_utf32, char32_t> conv;
  return conv.from_bytes(text);
}

Str utf32_to_8(cr<utf32> text) {
  using utf32_to_utf8 = std::codecvt_utf8_utf16<char32_t>;
  std::wstring_convert<utf32_to_utf8, char32_t> convert;
  return convert.to_bytes(text);
}
