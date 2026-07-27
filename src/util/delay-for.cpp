#include <chrono>
#include <cassert>
#include <thread>
#include "delay-for.hpp"
#include "util/macro.hpp"

bool wait_for(Cond_checker cond_checker, Delta_time timeout_sec, bool use_yield) {
  assert(cond_checker);
  cauto time_start = std::chrono::steady_clock::now();
  cauto timeout = std::chrono::duration<Delta_time>(timeout_sec);

  while (true) {
    return_if (cond_checker(), true);
    return_if (std::chrono::steady_clock::now() - time_start >= timeout, false);

    if (use_yield)
      std::this_thread::yield();
  }

  return false;
}
