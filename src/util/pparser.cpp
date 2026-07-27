#include <iostream>
#include <iomanip>
#include <stdexcept>
#include "pparser.hpp"

Pparser::Pparser(Pparser::v_param_t&& in) noexcept
: v_param{in} {}

void Pparser::operator ()(int argc, char** argv) const {
  if (argc < 2 && !skip_empty) {
    print_info();
    std::exit(EXIT_FAILURE);
  }
  auto tokens {get_tokens(argc, argv)};
  for (auto param: v_param) {
    auto opt {get_options(tokens, param.keys)};
    if (opt)
      param.action(*opt);
    if (!opt && param.needed) {
      Str msg {"need param for "};
      for (auto key: param.keys)
        msg += key + ' ';
      throw std::invalid_argument(msg);
    }
  } // for v_param
} // op (argv)

void Pparser::operator ()(cr<Str> opts) const {
  if (opts.empty() && !skip_empty) {
    print_info();
    std::exit(EXIT_FAILURE);
  }
  auto tokens {get_tokens(opts)};
  for (auto param: v_param) {
    auto opt {get_options(tokens, param.keys)};
    if (opt)
      param.action(*opt);
    if (!opt && param.needed) {
      print_info();
      
      Str msg {"need param for "};
      for (auto key: param.keys)
        msg += key + ' ';
      throw std::invalid_argument(msg);
    }
  } // for v_param
} // op (str)

std::optional<Str> Pparser::get_option(Strs tokens, cr<Str> option) const {
  auto itr {std::find(tokens.begin(), tokens.end(), option)};
  if (itr != tokens.end()) { // найден ключ
    if (++itr != tokens.end()) // найдена команда после ключа
      return *itr;
    return "";
  }
  return std::nullopt;
}

std::optional<Str> Pparser::get_options(Strs tokens,
cr<Strs> options) const {
  for (auto va: options) {
    auto ret {get_option(tokens, va)};
    if (ret)
      return ret;
  }
  return std::nullopt;
}

Strs Pparser::get_tokens(int argc, char** argv) const {
  Strs tokens;
  for (int i {1}; i < argc; ++i)
    tokens.push_back(argv[i]);
  return tokens;
}

Strs Pparser::get_tokens(cr<Str> opts) const {
  std::stringstream ss(opts);
  Str str;
  Strs tokens;
  while (getline(ss, str, ' '))
    tokens.push_back(str);
  return tokens;
}

void Pparser::print_info() const {
  std::cout << get_info();
  std::cout.flush();
}

Str Pparser::get_info() const {
  std::stringstream ss;

  ss << "Usage:\n";
  for (auto param: v_param) {
    for (auto key: param.keys)
      ss << key << ' ';
    ss << std::setw(12) << "\t: " << param.desc << "\n";
  }
  ss << "\n";

  return ss.str();
}
