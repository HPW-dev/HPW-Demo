#include <algorithm>
#include <utility>
#include <string>
#include <cstring>
#include "str-util.hpp"
#include "util/error.hpp"

void conv_sep(Str& str, char bad_separator, char good_separator) {
#ifdef DEBUG // в дебаге этот вар быстрее работает
  cfor (i, str.size())
    if (str[i] == bad_separator)
        str[i] = good_separator;
#else
  std::replace(str.begin(), str.end(), bad_separator, good_separator);
#endif
} // conv_sep

void conv_sep_for_archive(Str& str) { conv_sep(str, '\\', '/'); }

Strs split_str(CN<Str> str, char separator) {
  if (str.empty())
    return {};
  Strs ret = {};
  std::stringstream ss(str);
  std::string token;
  while (std::getline(ss, token, separator))
    if ( !token.empty())
      ret.push_back(token);
  return ret;
}

Str str_toupper(Str str) {
  std::for_each(str.begin(), str.end(), [](char& c) { c = ::toupper(c); });
  return str;
}

Str str_tolower(Str str) {
  std::for_each(str.begin(), str.end(), [](char& c) { c = ::tolower(c); });
  return str;
}

Str get_word(CN<Str> src, char tag1, char tag2) {
  std::size_t tag1_pos = src.find(tag1);
  std::size_t tag2_pos = src.rfind(tag2);
  if (tag1_pos == str_npos || tag2_pos == str_npos)
    return {};
  ++tag1_pos;
  return src.substr(tag1_pos, tag2_pos - 1);
}

std::pair<Str, Str> get_param_value(CN<Str> src, char op_ch, char cut_ch) {
  if (src.empty())
    return {};
  std::pair<Str, Str> ret = {};
  auto op_pos = src.find(op_ch);
  if (op_pos == str_npos)
    return {};
  uint i = op_pos;
  while (i > 0) {
    --i;
    if (src[i] != cut_ch) {
      ret.first = src.substr(0, i + 1);
      break;
    }
  } // while i > 0
  i = op_pos;
  while (i != src.size()) {
    ++i;
    if (src[i] != cut_ch) {
      ret.second = src.substr(i, src.size());
      break;
    }
  }
  return ret;
} // get_param_value

void remove_all(Str& str, char ch)
  { str.erase(std::remove(str.begin(), str.end(), ch), str.end()); }

void delete_all(Str& str, CN<Str> val) {
  while (1) {
    if (cauto pos = str.find(val); pos != str_npos)
      str.erase(pos, val.length());
    else
      return;
  }
} // delete_all

void replace_all(Str& str, CN<Str> val, CN<Str> new_) {
#ifdef DEBUG
  iferror(val == str, "replace_all: need different args \"" + val +
    "\", \"" + new_ + "\"");
#endif
  while (1) {
    if (cauto pos = str.find(val); pos != str_npos) {
      str.erase(pos, val.length());
      str.insert(pos, new_);
    } else {
      break;
    }
  }
} // replace_all

Vector<Cstr> sv_to_cv_fast(CN<Strs> sv) {
  auto size {sv.size()};
  Vector<Cstr> cv(size);
  cfor (i, size)
    cv[i] = sv[i].c_str();
  return cv;
}

Str strs_to_null_terminated(CN<Strs> strs) {
  Str ret;
  for (cnauto str: strs)
    ret += str + '\0';
  return ret;
}

Str addr_hash_to_str(CP<void> addr) {
  return n2s(std::hash<CP<void>>{}(addr));
}

Str cstr_to_cxxstr(Cstr cstr) { return Str(cstr, cstr + std::strlen(cstr)); }
