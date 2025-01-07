#include <cassert>
#include <cstring>
#include <iostream>
#include "util/log.hpp"
#include "util/str-util.hpp"
#include "util/error.hpp"
#include "util/pparser.hpp"
#include "util/platform.hpp"

/*
#include "util/net/udp-server.hpp"
#include "util/net/udp-client.hpp"
*/
#include "util/net/udp-mgr.hpp"

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
  hpw_log("packet from address: \"" + packet.source_address + "\"\n");
  hpw_log("message size: " + n2s(packet.bytes.size()) + "\n");
  hpw_log("message: " + Str(cptr2ptr<Cstr>(packet.bytes.data())) + "\n");
}

void server_test(cr<Args> args) {
  hpw_info("Server test\n");

  #ifndef DEBUG
  hpw_info("timer calibration...\n");
  calibrate_delay(0.1);
  #endif

  hpw_info("server init...\n");
  net::Udp_mgr udp;
  udp.start_server(s2n<u16_t>(args.port));

  std::stringstream avaliable_ips;
  avaliable_ips << "avaliable IPv4's:\n";
  for (crauto ip: udp.avaliable_ipv4s())
    avaliable_ips << ip << "\n";
  avaliable_ips << "\n";
  hpw_info(avaliable_ips.str());

  if (args.async)
    udp.run_packet_listening();

  hpw_info("server loop:\n");
  while (true) {
    udp.update();

    if (args.async) {
      if (udp.has_packets()) {
        for (crauto packet: udp.packets())
          if (packet.loaded_correctly)
            print_packet(packet);
        udp.clear_packets();
      } else {
        hpw_debug("nop\n");
      }

      constexpr Seconds DELAY = 1.0;
      delay_sec(DELAY);
    } else { // serial mode
      print_packet(udp.wait_packet());
    }
  }
}

void client_test(cr<Args> args) {
  /*
  hpw_log("Client test\n");
  assert(!args.ip.empty());

  hpw_log("init client...\n");
  net::Udp_mgr udp;
  udp.start_client(args.ip, s2n<u16_t>(args.port));

  hpw_log("connect to server...\n");
  udp.try_to_connect();

  while (true) {
    Str message;
    std::cout << "enter message (\"exit\" for exit) > ";
    std::getline(std::cin, message);
    std::cin >> message;

    // подготовка пакета с сообщение к отправке
    Bytes data(message.size() + 1);
    std::memcpy(ptr2ptr<void*>(data.data()), cptr2ptr<cp<void>>(message.data()), message.size());
    data.back() = scast<byte>('\0');

    if (args.async)
      udp.async_send(data);
    else
      udp.send(data);

    udp.update();
    break_if(str_tolower(message) == "exit");
  }
  */
}

int main(int argc, char** argv) {
  hpw_log("Network test\n\n");

  cauto args = parse_args(argc, argv);
  if (args.is_server)
    server_test(args);
  else
    client_test(args);
}
