#include <cassert>
#include "util/error.hpp"
#include "util/log.hpp"
#include "util/str-util.hpp"
#include "util/pparser.hpp"
#include "util/net/udp-mgr.hpp"

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
  argparser(argc, argv);

  hpw_info(Str("server mode: ") + (ret.is_server ? "enabled" : "disabled") + "\n");
  assert(!ret.port.empty());
  hpw_info("port: " + ret.port + "\n");
  if (!ret.ip.empty())
    hpw_info("ip: " + ret.ip + "\n");
  
  return ret;
}

int main(int argc, char** argv) {
  hpw_info("Rollback test\n\n");

  cauto args = parse_args(argc, argv);
  net::Udp_mgr udp;
  if (args.is_server) {
    hpw_info("start server...\n");
    udp.start_server(s2n<u16_t>(args.port));
  } else {
    hpw_info("start client...\n");
    udp.start_client(args.ip, s2n<u16_t>(args.port));
  }

  // TODO

  hpw_info("rollback test end\n");
}
