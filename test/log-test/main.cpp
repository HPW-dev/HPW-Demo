#include "util/log.hpp"

int main() {
  log_info << "info text test";
  log_warning << "warning text test";
  log_error << "error text test";
  log_debug << "debug text test";
  log_null << "null text test";
  log_info << "more " << "more " << "more " << "more";
  
  hpw::reopen_log_file("delme.txt");
  hpw::logger.config.use_terminal = false;
  log_info << "output to log file test";
  log_error << "output to log file test";
  log_warning << "output to log file test";
  log_debug << "output to log file test";
  log_null << "output to log file test";
}
