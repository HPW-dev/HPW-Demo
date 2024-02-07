#include <string_view>
#include "error.hpp"

namespace hpw {

Error::Error (CN<Str> _msg, CN<Str> _location)
  : location(_location)
  , msg{_msg} {}

Cstr Error::what() const noexcept {
  last_err = "in " + location + ": " + msg;
  return last_err.c_str();
}

CN<decltype(Error::msg)> Error::get_msg() const { return msg; }
CN<decltype(Error::location)> Error::get_location() const { return location; }

} // hpw ns
