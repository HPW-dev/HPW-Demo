#include <cassert>
#include <iostream>
#include "util/log.hpp"
#include "util/str-util.hpp"
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
  argparser(argc, argv);

  hpw_log(Str("server mode: ") + (ret.is_server ? "enabled" : "disabled") + "\n");
  assert(!ret.port.empty());
  hpw_log("port: " + ret.port + "\n");
  if (!ret.ip.empty())
    hpw_log("ip: " + ret.ip + "\n");
  
  return ret;
}

void server_test(cr<Str> port) {
  hpw_log("Server test\n");
}

void client_test(cr<Str> ip, cr<Str> port) {
  hpw_log("Client test\n");

  while (true) {
    Str message;
    std::cout << "enter message (\"exit\" for exit) > ";
    std::getline(std::cin, message);
    std::cin >> message;

    break_if(str_tolower(message) == "exit");
  }
}

int main(int argc, char** argv) {
  hpw_log("Network test\n\n");

  cauto args = parse_args(argc, argv);
  if (args.is_server)
    server_test(args.port);
  else
    client_test(args.ip, args.port);
}
