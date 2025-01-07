#include <cassert>
#include "util/log.hpp"
#include "util/error.hpp"
#include "util/pparser.hpp"

struct Args {
  bool is_server {};
  Str ip, port;
};

Args parse_args(int argc, char** argv) {
  Args ret;
  Pparser argparser({
    {{"-c", "--client"}, "set client mode", [&](cr<Str> arg) {ret.is_server = false;}},
    {{"-s", "--server"}, "set server mode", [&](cr<Str> arg) {ret.is_server = true;}},
    {{"-i", "--ip"}, "set IP (--ip 127.0.0.1)", [&](cr<Str> arg) {ret.ip = arg;}},
    {{"-p", "--port"}, "port number (--port 49099)", [&](cr<Str> arg) {ret.port = arg;}, true},
  });

  iferror (argc <= 1, argparser.get_info());

  hpw_log(Str("server mode: ") + (ret.is_server ? "enabled" : "disabled") + "\n");
  assert(!ret.port.empty());
  hpw_log("port: " + ret.port + "\n");
  if (!ret.ip.empty())
    hpw_log("ip: " + ret.ip + "\n");
  
  return ret;
}

int main(int argc, char** argv) {
  hpw_log("Network test\n\n");

  cauto args = parse_args(argc, argv);
}
