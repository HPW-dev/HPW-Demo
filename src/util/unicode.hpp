#pragma once
// @file конвертирование строк
#include <type_traits>
#include <algorithm>
#include <sstream>
#include <vector>
#include <string>
#include <cstring>
#include "util/macro.hpp"
#include "util/vector-types.hpp"
#include "util/hpw-concept.hpp"
#include "util/str.hpp"

using utf8 = std::u8string;
using utf32 = std::u32string;

// string convert
template <class OUT, have_begin_end IN>
OUT sconv(cr<IN> src)
  { return OUT(src.begin(), src.end()); }

// other c-string style convert
template <class OUT, class IN>
OUT sconv(const IN* src) requires std::is_fundamental<IN>::value
  { return OUT(src, src + std::strlen(src)); }

// c-string convert
template <class OUT, typename in_char_t>
OUT sconvc(in_char_t src) {
  cauto len = strlen(src);
  if (len == 0)
    return {};
  return OUT(src, src + len);
}

template <class STR_T> STR_T
yn2s(bool comp)
  { return comp ? sconvc<STR_T>("YES") : sconvc<STR_T>("NO"); }

// num -> str
template <class OUT, typename NUM_T>
OUT n2s(cr<NUM_T> num) {
  cauto str_num = std::to_string(num);
  cauto ret = sconv<OUT>(str_num);
  return ret;
}

// num -> str + precission len
template <class OUT, typename NUM_T>
OUT n2s(cr<NUM_T> num, int prec) {
  std::ostringstream out;
  out.precision(prec);
  out << std::fixed << num;
  return sconv<OUT>(out.str());
}

// делит строку в местах разделительного символа
template <class str_t, typename ch_t>
Vector<str_t> split(cr<str_t> str, ch_t separator) {
  Vector<str_t> ret;
  std::basic_stringstream<ch_t> ss(str);
  str_t token;
  while (std::getline(ss, token, separator))
    if ( !token.empty())
      ret.push_back(token);
  return ret;
} // split

// удаляет символы ch из строки
template <class str_t, typename ch_t>
void remove_all(str_t& str, ch_t ch)
  { str.erase(std::remove(str.begin(), str.end(), ch), str.end()); }

template <class str_t, typename ch_t>
void replace_all(str_t& str, ch_t find_it, ch_t replace_to) {
  std::transform( str.begin(), str.end(), str.begin(),
    [find_it, replace_to](cr<ch_t> src) {
      return src == find_it ? replace_to : src;
    } );
}

// строку в число
template <class str_t, typename T>
T s2n(cr<str_t> str, auto base = std::hex) {
  using ch_t = typename str_t::value_type;
  std::basic_stringstream<ch_t> ss;
  ss << base << str;
  T ret;
  ss >> ret;
  return ret;
}

// перекодирование из utf8 строки в utf32
utf32 utf8_to_32(cr<Str> text);
// перекодирование из utf32 строки в utf8
Str utf32_to_8(cr<utf32> text);
