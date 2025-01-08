#include <cassert>
#include "util/error.hpp"
#include "util/log.hpp"
#include "util/str-util.hpp"
#include "util/pparser.hpp"
#include "util/platform.hpp"
#include "util/net/udp-mgr.hpp"
#include "util/math/num-types.hpp"

constx Delta_time TICK_TIME = 1.0 / 60.0;

struct Player {
  Str address {};
  bool is_server {};
};

static inline Player g_player2 {};

enum class Packet_tag: byte {
  error = 0,
  hello,
};

struct Packet_hello {
  Packet_tag tag {Packet_tag::hello};
  bool is_server {};
};

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

// ждать соединения с другим игроком, вернуть его адрес
void find_player(net::Udp_mgr& udp) {
  assert(udp);

  Str address;
  constexpr Seconds delay = 0.5;
  int timeout = 30;
  Bytes packet_data(sizeof(Packet_hello));
  Packet_hello& packet_hello = rcast<Packet_hello&>(*packet_data.data());
  packet_hello = Packet_hello{};
  packet_hello.is_server = udp.is_server();

  while (timeout) {
    if (udp.is_client()) {
      hpw_info("send hello message to server...\n");
      udp.async_send(packet_data);
    } else {
      hpw_info("wait hello messages from client...\n");
      cauto packet = udp.load_packet_if_exist();

      if (packet) {
        hpw_info("data loaded\n");
        if (packet->bytes.size() >= sizeof(Packet_hello)) {
          cr<Packet_hello> hello_2 = rcast<cr<Packet_hello>>(*(packet->bytes.data()));
          if (hello_2.tag == Packet_tag::hello) {
            hpw_info("hello tag finded\n");
            g_player2.address = packet->source_address;
            g_player2.is_server = hello_2.is_server;
            hpw_info("address: " + g_player2.address + "\n");
            hpw_info("is server: " + Str(g_player2.is_server ? "yes" : "no") + "\n");
            // TODO anwerr
            return;
          } else {
            hpw_info("is not hello packet\n");
          }
        } else {
          hpw_info("small packet (" + n2s(packet->bytes.size())
              + "/" + n2s(sizeof(Packet_hello)) + " bytes)\n");
        }
      } else {
        hpw_info("no message\n");
      }
    }
    
    delay_sec(delay);
    --timeout;
  }

  iferror (timeout <= 0, "connection timeout");
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

  hpw_info("calibrate timer...\n");
  calibrate_delay(TICK_TIME);

  hpw_info("find player...\n");
  find_player(udp);

  hpw_info("rollback test end\n");
}
