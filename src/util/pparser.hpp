#pragma once
#include <optional>
#include <functional>
#include "macro.hpp"
#include "str.hpp"
#include "vector-types.hpp"

// parsing cmd line args
class Pparser {
public:
  struct Param {
    Strs keys {};
    Str desc {};
    std::function<void (cr<Str>)> action {};
    bool needed {false};
  };
  using v_param_t = Vector<Param>;
  bool skip_empty {false}; ///< игнор пустого ввода

  // @param in vector of: {{"-i", "--input", ...}, "description", lambda <void (cr<Str>)>, is_needed}
  explicit Pparser(v_param_t&& in) noexcept;
  ~Pparser() = default;
  void operator ()(int argc, char** argv) const;
  void operator ()(cr<Str> opts) const;
  void print_info() const;

private:
  v_param_t v_param {};

  // вернет nullopt если не нашли ключ
  std::optional<Str> get_option(Strs tokens, cr<Str> option) const;
  std::optional<Str> get_options(Strs tokens, cr<Strs> options) const;
  Strs get_tokens(int argc, char** argv) const;
  Strs get_tokens(cr<Str> opts) const;
};
