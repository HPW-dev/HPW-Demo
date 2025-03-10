#include <string_view>
#include "error.hpp"

namespace hpw {

Error::Error (cr<Str> _msg, cr<Str> _location)
  : location(_location)
  , msg{_msg} {}

Cstr Error::what() const noexcept {
  last_err = location + ": " + msg;
  return last_err.c_str();
}

cr<decltype(Error::msg)> Error::get_msg() const { return msg; }
cr<decltype(Error::location)> Error::get_location() const { return location; }

} // hpw ns
