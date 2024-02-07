#pragma once
#include <utility>
#include <sstream>
#include <cmath>
#include "util/macro.hpp"
#include "util/platform.hpp"
#include "util/str.hpp"
#include "util/math/num-types.hpp"

/// замена разделительных символов на совместимые с системой
void conv_sep(Str& str, char bad_separator=NSEPARATOR,
  char good_separator=SEPARATOR);
void conv_sep_for_archive(Str& str);
/// делит строку в местах разделительного символа
Strs split_str(CN<Str> str, char separator);
/// делает все буквы строки БОЛЬШИМИ
Str str_toupper(Str str);
/// делает все буквы строки меньше
Str str_tolower(Str str);
/// возвращает слово между двумя символами
Str get_word(CN<Str> src, char tag1, char tag2);
/// удаляет символы ch из строки str
void remove_all(Str& str, char ch);
/// удаляет все val строки из str
void delete_all(Str& str, CN<Str> val);
/// заменяет все подстроки val из str на new_
void replace_all(Str& str, CN<Str> val, CN<Str> new_);
/** парсит параметр/зрачение между символом op_ch.
вырезает cut_ch между ними */
std::pair<Str, Str> get_param_value(CN<Str> src, char op_ch, char cut_ch);
/// string vector to C-style string vector (без выделения под строки)
Vector<Cstr> sv_to_cv_fast(CN<Strs> sv);
/// vector strings to compared string with \0 separators
Str strs_to_null_terminated(CN<Strs> strs);
/// конвертирует сишную строку в std::string
Str cstr_to_cxxstr(Cstr cstr);

/// преобразовать адрес в строку
template <class T>
Str a2s(const T* ptr) {
  if ( !ptr)
    return "null";
  std::stringstream strm;
  strm << ptr;
  return strm.str();
}

/// convert num to string
template<typename T>
Str n2s(T num) { return std::to_string(num); }

/// convert num to string + precission
template<typename T>
Str n2s(T num, int prec) {
  std::ostringstream out;
  out.precision(prec);
  out << std::fixed << num;
  return out.str();
}

template<typename T>
Str n2hex(T num) {
  std::ostringstream out;
  out << std::hex << num;
  return out.str();
}

template<typename T>
T s2n(Str str) {
  std::istringstream ss(str);
  T ret;
  ss >> ret;
  return ret;
}

/// переводит секунды в MM:SS.SS
template<class T>
Str s2t(T val) {
  cauto sec {std::fmod(val, 60.0)};
  cauto min {scast<int>(val * (1.0 / 60.0))};
  return n2s(min) + ":" + n2s(sec, 2);
}

/// преобразует из массива оканчивающимся на 0, в OUT
template <class OUT, class IN>
OUT from_null_ended(CN<IN> in) {
  OUT ret;
  for (cnauto ch: in) {
    if (ch == '\0')
      break;
    ret += ch;
  }
  return ret;
}

/// преобразует строку в массива символов в стиле строк C
template<class OUT, class IN>
void to_null_ended(OUT &dst, CN<IN> src) {
  uint i {0};
  for (cnauto ch: src) {
    dst[i] = ch;
    ++i;
  }
  dst[i] = '\0';
}

Str addr_hash_to_str(CP<void> addr);
