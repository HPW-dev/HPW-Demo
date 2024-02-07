#pragma once

#ifdef DEBUG
#include <atomic>
#include <mutex>
#include <syncstream>
#include <iostream>

namespace debug_ns {
  inline std::atomic_bool hpw_log_no_details {false};
}

#define hpw_log(MSG) { \
  std::osyncstream synced_out(std::cout); \
  if (debug_ns::hpw_log_no_details) \
    synced_out << MSG; \
  else \
    synced_out << __FILE__ << ':' << __LINE__ << ": " << MSG; \
}
#define iflog(COND, MSG) { \
  if (COND) \
    hpw_log (MSG) \
}
#else
#define hpw_log(MSG) {}
#define iflog(COND, MSG) {}
#endif

#ifdef DETAILED_LOG
#define detailed_log(MSG) hpw_log(MSG)
#define detailed_iflog(COND, MSG) iflog(COND, MSG)
#else
#define detailed_log(MSG) {}
#define detailed_iflog(COND, MSG) {}
#endif
