#include "unicode.hpp"
#include "util/error.hpp"

utf32 utf8_to_32(cr<Str> utf8_str) {
  utf32 result;
  size_t i = 0;
  size_t len = utf8_str.length();
    
  while (i < len) {
    utf32::value_type cp = 0;
    unsigned char c = utf8_str[i];
        
    // 1-byte sequence (0xxxxxxx)
    if (c <= 0x7F) {
      cp = c;
      i += 1;
    }
    // 2-byte sequence (110xxxxx 10xxxxxx)
    else if ((c & 0xE0) == 0xC0) {
      iferror (i + 1 >= len, "Invalid UTF-8 sequence");
      cp = ((c & 0x1F) << 6) | (utf8_str[i+1] & 0x3F);
      i += 2;
    }
    // 3-byte sequence (1110xxxx 10xxxxxx 10xxxxxx)
    else if ((c & 0xF0) == 0xE0) {
      iferror(i + 2 >= len, "Invalid UTF-8 sequence");
      cp = ((c & 0x0F) << 12) | ((utf8_str[i+1] & 0x3F) << 6) | (utf8_str[i+2] & 0x3F);
      i += 3;
    }
    // 4-byte sequence (11110xxx 10xxxxxx 10xxxxxx 10xxxxxx)
    else if ((c & 0xF8) == 0xF0) {
      iferror(i + 3 >= len, "Invalid UTF-8 sequence");
      cp = ((c & 0x07) << 18) | ((utf8_str[i+1] & 0x3F) << 12) 
        | ((utf8_str[i+2] & 0x3F) << 6) | (utf8_str[i+3] & 0x3F);
      i += 4;
    } else {
      error("Invalid UTF-8 sequence");
    }
        
    // Validate the code point
    if (cp > 0x10FFFF || (cp >= 0xD800 && cp <= 0xDFFF)) {
      error("Invalid Unicode code point");
    }    
    result.push_back(cp);
  }
    
  return result;
}

Str utf32_to_8(cr<utf32> text) {
  Str ret;
    
  for (utf32::value_type c : text) {
    if (c <= 0x7F) {
      ret.push_back(scast<char>(c));
    } else if (c <= 0x7FF) {
      ret.push_back(scast<char>(0xC0 | ((c >> 6) & 0x1F)));
      ret.push_back(scast<char>(0x80 | (c & 0x3F)));
    } else if (c <= 0xFFFF) {
      ret.push_back(scast<char>(0xE0 | ((c >> 12) & 0x0F)));
      ret.push_back(scast<char>(0x80 | ((c >> 6) & 0x3F)));
      ret.push_back(scast<char>(0x80 | (c & 0x3F)));
    } else if (c <= 0x10FFFF) {
      ret.push_back(scast<char>(0xF0 | ((c >> 18) & 0x07)));
      ret.push_back(scast<char>(0x80 | ((c >> 12) & 0x3F)));
      ret.push_back(scast<char>(0x80 | ((c >> 6) & 0x3F)));
      ret.push_back(scast<char>(0x80 | (c & 0x3F)));
    } else {
      error("invalid character");
    }
  }
    
  return ret;
}
