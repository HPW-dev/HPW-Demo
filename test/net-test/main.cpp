#include <cassert>
#include <iostream>
#include "util/log.hpp"
#include "util/str-util.hpp"
#include "util/error.hpp"
#include "util/pparser.hpp"
#include "util/platform.hpp"
#include "util/net/udp-server.hpp"

struct Args {
  bool is_server {};
  bool async {};
  Str ip, port;
};

Args parse_args(int argc, char** argv) {
  Args ret;
  Pparser argparser({
    {{"-c", "--client"}, "set client mode", [&](cr<Str> arg) {ret.is_server = false;}},
    {{"-s", "--server"}, "set server mode", [&](cr<Str> arg) {ret.is_server = true;}},
    {{"-a", "--async"}, "set async mode", [&](cr<Str> arg) {ret.async = true;}},
    {{"-i", "--ip"}, "set IP (--ip 127.0.0.1)", [&](cr<Str> arg) {ret.ip = arg;}},
    {{"-p", "--port"}, "port number (--port 49099)", [&](cr<Str> arg) {ret.port = arg;}, true},
  });

  iferror (argc <= 1, argparser.get_info());
  argparser(argc, argv);

  hpw_log(Str("server mode: ") + (ret.is_server ? "enabled" : "disabled") + "\n");
  hpw_log(Str("async mode: ") + (ret.async ? "enabled" : "disabled") + "\n");
  assert(!ret.port.empty());
  hpw_log("port: " + ret.port + "\n");
  if (!ret.ip.empty())
    hpw_log("ip: " + ret.ip + "\n");
  
  return ret;
}

void print_packet(cr<net::Packet> packet) {
  // TODO
  error("need impl");
}

void server_test(cr<Args> args) {
  hpw_info("Server test\n");

  hpw_info("timer calibration...\n");
  calibrate_delay(0.1);

  hpw_info("server init...\n");
  net::Udp_server srv(s2n<u16_t>(args.port));

  std::stringstream avaliable_ips;
  avaliable_ips << "avaliable IPv4's:\n";
  for (crauto ip: srv.avaliable_ipv4s())
    avaliable_ips << ip << "\n";
  avaliable_ips << "\n";
  hpw_info(avaliable_ips.str());

  hpw_info("server loop:\n");
  while (true) {
    if (args.async) {
      if (srv.has_packets()) {
        for (crauto data: srv.packets())
          print_packet(data);
        srv.clear_packets();
      } else {
        hpw_debug("nop\n");
      }

      constexpr Seconds DELAY = 1.0;
      delay_sec(DELAY);
    } else { // serial mode
      error("need impl");
    }
  }
}

void client_test(cr<Args> args) {
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
    server_test(args);
  else
    client_test(args);
}
