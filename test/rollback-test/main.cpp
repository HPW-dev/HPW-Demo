#include <cassert>
#include <cstring>
#include <unordered_set>
#include "util/error.hpp"
#include "util/log.hpp"
#include "util/str-util.hpp"
#include "util/pparser.hpp"
#include "util/platform.hpp"
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

/*
struct Player {
  Str address {};
  u16_t port {};
  bool is_server {};
  bool is_ready {};
};

static inline Player g_player2 {};

enum class Packet_tag: byte {
  error = 0,
  ready,
  ping,
  end,
};

struct Packet_ready {
  Packet_tag tag {Packet_tag::ready};
  bool is_server {};
  bool is_ready {};
};

struct Packet_end {
  Packet_tag tag {Packet_tag::end};
  bool is_end {};
};

struct Packet_ping {
  Packet_tag tag {Packet_tag::ping};
  std::uint16_t id {};
  Seconds sending_time {};
  bool arrived {};
};

template <class T>
void async_send(net::Udp_mgr& udp, cr<T> src, cr<Str> ip={}, u16_t port={}) {
  static_assert(sizeof(T) > 0);
  static_assert(sizeof(T) < net::PACKET_BUFFER_SZ);
  assert(udp);

  constexpr auto packet_sz = sizeof(T);
  Bytes data(packet_sz);
  std::memcpy(ptr2ptr<void*>(data.data()), cptr2ptr<cp<void>>(&src), packet_sz);
  if (ip.empty())
    udp.async_send(data);
  else
    udp.async_send(data, {}, ip, port);
}

// проверяет что игрок готов
bool check_if_ready(cr<Bytes> bytes) {
  assert(!bytes.empty());

  constexpr auto packet_sz = sizeof(Packet_ready);
  if (bytes.size() == packet_sz) {
    crauto in = rcast<cr<Packet_ready>>(*bytes.data());
    if (in.tag != Packet_tag::ready) {
      hpw_info("тег не совпадает\n");
      return false;
    }

    if (in.is_ready) {
      g_player2.is_server = in.is_server;
      g_player2.is_ready = true;
      return true;
    }
  }

  hpw_info("пакет не является Packet_ready\n");
  return false;
}

// ждать соединения с другим игроком
void try_to_connect(net::Udp_mgr& udp, cr<Args> args) {
  assert(udp);

  hpw_info("trying to connect...\n");
  uint timeout = 50;
  constexpr Seconds DELAY = 0.5;
  Packet_ready ready_packet;
  ready_packet.is_ready = true;
  ready_packet.is_server = udp.is_server();
  
  while (timeout-- > 0) {
    if (udp.is_client())
      async_send(udp, ready_packet);
    elif (g_player2.is_ready)
      async_send(udp, ready_packet, g_player2.address, g_player2.port);

    cauto input_packet = udp.load_packet_if_exist();
    if (input_packet && check_if_ready(input_packet->bytes)) {
      hpw_info("получен ответ\n");
      g_player2.address = input_packet->source_address;
      g_player2.port = s2n<u16_t>(args.port);
      break;
    }

    delay_sec(DELAY);
  }

  iferror (timeout <= 0, "connection timeout");
  hpw_info (
    "connected player info:\n"
    "  is " + Str(g_player2.is_server ? "server" : "client") + "\n"
    "  is " + Str(g_player2.is_ready ? "ready" : "not ready") + "\n"
    "  address: " + g_player2.address + "\n"
  );
}

void print_ping(cr<Bytes> bytes) {
  assert(!bytes.empty());

  constexpr auto packet_sz = sizeof(Packet_ping);
  if (bytes.size() == packet_sz) {
    crauto in = rcast<cr<Packet_ping>>(*bytes.data());

    if (in.tag != Packet_tag::ping) {
      hpw_info("тег не совпадает\n");
      return;
    }

    if (in.arrived) {
      hpw_info(
        "packet ping info:\n"
        "  id: " + n2s(in.id) + "\n"
        "  delay: " + n2s(get_cur_time() - in.sending_time) + "\n"
      );
    } else {
      hpw_info("этот пинг-пакет не был доставлен\n");
    }

    return;
  }

  hpw_info("пакет не является Packet_ping\n");
}

void calculate_ping(net::Udp_mgr& udp) {
  assert(udp);
  hpw_log("calculate ping...\n");

  if (udp.is_server()) {
    constexpr std::uint16_t PACKETS = 100;
    cfor (packet_id, PACKETS) {
      Packet_ping packet;
      packet.id = packet_id;
      packet.sending_time = get_cur_time();
      async_send(udp, packet, g_player2.address, g_player2.port);
    }

    uint timeout = 50;
    while (timeout-- > 0) {
      cauto packet = udp.load_packet_if_exist();
      if (packet)
        print_ping(packet->bytes);
    }
  } else { // client
    cauto packet = udp.load_packet_if_exist();
    constexpr auto packet_sz = sizeof(Packet_ping);
    if (packet && packet->bytes.size() == packet_sz) {
      auto in = rcast<cr<Packet_ping>>(*packet->bytes.data());
      if (in.tag == Packet_tag::ping) {
        in.arrived = true;
        async_send(udp, in);
        hpw_log("пинг пакет переотправлен\n");
      }
    }
  }
}
*/

void wait_connections(cr<Args> args) {
  hpw_info("create TCP server...\n");
  net::Tcp_mgr tcp;
  tcp.start_server(s2n<u16_t>(args.port));

  constexpr Seconds TIMEOUT = 5;
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

  /*
  net::Udp_mgr udp;
  if (args.is_server) {
    hpw_info("start server...\n");
    udp.start_server(s2n<u16_t>(args.port));
  } else {
    hpw_info("start client...\n");
    udp.start_client(args.ip, s2n<u16_t>(args.port));
  }

  try_to_connect(udp, args);
  calculate_ping(udp);
  */

  hpw_info("rollback test end\n");
}
