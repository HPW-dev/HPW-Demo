#include <cassert>
#include <cstring>
#include <unordered_set>
#include "util/error.hpp"
#include "util/log.hpp"
#include "util/str-util.hpp"
#include "util/pparser.hpp"
#include "util/platform.hpp"
#include "util/net/udp-packet-mgr.hpp"
#include "util/net/udp-mgr.hpp"
#include "util/net/tcp-mgr.hpp"
#include "util/math/num-types.hpp"

constx Delta_time TICK_TIME = 1.0 / 60.0;

// кто смог подключиться к серверу
static Strs g_connected_ipv4s;

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

void print_interfaces(net::Tcp_mgr& tcp) {
  Str ret = "avaliable interfaces:\n";
  cauto ips = tcp.avaliable_ipv4s();
  for (crauto ip: ips)
    ret += "  " + ip + "\n";
  if (ips.empty())
    ret += "  empty\n";
  hpw_info(ret);
}

void wait_connections(cr<Args> args) {
  hpw_info("create TCP server...\n");
  net::Tcp_mgr tcp;
  tcp.start_server(s2n<u16_t>(args.port));
  print_interfaces(tcp);

  constexpr Seconds TIMEOUT = 10;
  std::unordered_set<Str> connected_ipv4s; // для уникальных IP
  hpw_info("wait connections for " + n2s(TIMEOUT) + " seconds\n");

  // ждём несколько секунд соединения:
  cauto start_time = get_cur_time();
  std::atomic_bool connected {};
  Str incoming_ipv4;
  tcp.async_find_incoming_ipv4(connected, incoming_ipv4);

  while (get_cur_time() - start_time < TIMEOUT) {
    // если получили соединение, засейвить и продолжить искать новое
    if (connected) {
      hpw_info("incoming ip: " + incoming_ipv4 + "\n");
      connected_ipv4s.emplace(incoming_ipv4);
      connected = false;
      tcp.async_find_incoming_ipv4(connected, incoming_ipv4);
    }

    tcp.update();
  }

  // передать уникальные ip
  for (rauto ip: connected_ipv4s)
    g_connected_ipv4s.emplace_back(std::move(ip));
  hpw_info("connected ips:\n");
  for (crauto ip: g_connected_ipv4s)
    hpw_info("  " + ip + "\n");
  if (g_connected_ipv4s.empty())
    hpw_info("  empty\n");
}

void try_to_connect(cr<Args> args) {
  hpw_info("create TCP client...\n");

  hpw_info("connecting to TCP server...\n");
  net::Tcp_mgr tcp;
  tcp.start_client(args.ip, s2n<u16_t>(args.port));
  print_interfaces(tcp);

  constexpr Seconds TIMEOUT = 5;
  hpw_info("wait connection for " + n2s(TIMEOUT) + " seconds\n");
  std::atomic_bool connected {};
  tcp.async_connect(connected);

  cauto start_time = get_cur_time();
  while (get_cur_time() - start_time < TIMEOUT) {
    tcp.update();
    break_if (connected);
  }

  hpw_info("client " + Str(connected ? "connected" : "not connected") + " to server\n");
}

void udp_packet_mgr_test(cr<Args> args) {
  assert(!args.port.empty());
  return_if(args.is_server && args.ip.empty());
  hpw_info("UDP packet manager test start...\n");

  net::Udp_packet_mgr udppm;
  if (args.is_server)
    udppm.start_server(s2n<u16_t>(args.port));
  else
    udppm.start_client(args.ip, s2n<u16_t>(args.port));
  
  hpw_info("UDP packet manager loop start...\n");
  bool is_ran = true;
  while (is_ran) {
    udppm.update();
  }

  hpw_info("UDP packet manager test end\n");
}

int main(int argc, char** argv) {
  hpw_info("Rollback test\n\n");

  cauto args = parse_args(argc, argv);

  hpw_info("calibrate timer...\n");
  calibrate_delay(TICK_TIME);

  if (args.is_server) {
    wait_connections(args);
  } else {
    try_to_connect(args);
  }

  // TODO del:
  udp_packet_mgr_test(args);

  hpw_info("rollback test end\n");
}
