#pragma once
#include <sstream>
#include <stdexcept>
#include "macro.hpp"
#include "str.hpp"

namespace hpw {

class Error final: public std::exception {
  const Str location {};
  const Str msg {};
  mutable Str last_err {};

public:
  Error() = default;
  ~Error() = default;

  explicit Error (CN<Str> _msg, CN<Str> _location={});
  Cstr what() const noexcept override;
  CN<decltype(msg)> get_msg() const;
  CN<decltype(location)> get_location() const;
}; // Error

} // hpw ns

#define error(msg) { \
  std::stringstream location; \
  std::stringstream msg_stream; \
  location << __FILE__ << ':' << __LINE__; \
  msg_stream << msg; \
  throw hpw::Error(msg_stream.str(), location.str()); \
}

#define iferror(cond, msg) \
if (cond) { \
  error(msg) \
}

#define check_p(ptr) iferror( !ptr, #ptr " is null")
#define hpw_assert(e) iferror( !(e), #e)
