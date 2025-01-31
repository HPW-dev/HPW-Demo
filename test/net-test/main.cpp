#include "util/net/net-packet-mgr.hpp"
#include "util/pparser.hpp"
#include "util/log.hpp"
#include "util/str-util.hpp"

struct Connection_info {
  net::Port source_udp_port = net::SERVER_UDP_PORT;
  net::Port source_tcp_port = net::SERVER_TCP_PORT;
  net::Port target_udp_port = net::CLIENT_UDP_PORT;
  net::Port target_tcp_port = net::CLIENT_TCP_PORT;
  Str source_ip_v4 = "127.0.0.1";
  Str target_ip_v4 = "127.0.0.2";
  bool is_server {true};
};

static Connection_info _connection_info {};

void parse_args(int argc, char** argv) {
  Pparser arg_parser( {
    {{"-s", "--server"}, "sets \"server\' mode", [](cr<Str> arg) { _connection_info.is_server = true; }},
    {{"-c", "--client"}, "sets \"client\" mode", [](cr<Str> arg) { _connection_info.is_server = false; }},
    {{"--source_ip"}, "source IPv4", [](cr<Str> arg) { _connection_info.source_ip_v4 = arg; }},
    {{"--target_ip"}, "target IPv4", [](cr<Str> arg) { _connection_info.target_ip_v4 = arg; }},
    {{"--source_udp_port"}, "sets source UDP port", [](cr<Str> arg) { _connection_info.source_udp_port = s2n<net::Port>(arg); }},
    {{"--source_tcp_port"}, "sets source TCP port", [](cr<Str> arg) { _connection_info.source_tcp_port = s2n<net::Port>(arg); }},
    {{"--target_udp_port"}, "sets target UDP port", [](cr<Str> arg) { _connection_info.target_udp_port = s2n<net::Port>(arg); }},
    {{"--target_tcp_port"}, "sets target TCP port", [](cr<Str> arg) { _connection_info.target_tcp_port = s2n<net::Port>(arg); }},
  } );

  if (argc == 1)
    hpw_info("Usage:\n" + arg_parser.get_info());

  arg_parser.skip_empty = true;
  arg_parser(argc, argv);
}

void print_params() {
  std::stringstream txt;
  txt << "input params:\n";
  txt << "- mode: " << (_connection_info.is_server ? "server" : "client") << "\n";
  txt << "- source ip v4: " << _connection_info.source_ip_v4 << "\n";
  txt << "- target ip v4: " << _connection_info.target_ip_v4 << "\n";
  txt << "- port:\n";
  txt << "  * source:\n";
  txt << "    + UDP: " << _connection_info.source_udp_port << "\n";
  txt << "    + TCP: " << _connection_info.source_tcp_port << "\n";
  txt << "  * target:\n";
  txt << "    + UDP: " << _connection_info.target_udp_port << "\n";
  txt << "    + TCP: " << _connection_info.target_tcp_port << "\n";
  hpw_info(txt.str());
}

void init_pck_mgr(net::Packet_mgr& dst) {
  net::Packet_mgr::Config cfg;
  cfg.is_server = _connection_info.is_server;
  cfg.port_udp = _connection_info.source_udp_port;
  cfg.port_tcp = _connection_info.source_tcp_port;
  cfg.ip_v4 = _connection_info.source_ip_v4;
  dst.start(cfg);
}

void test_1(net::Packet_mgr& mgr) {
  hpw_info("test 1 - UDP sending byte to server\n");

  if (_connection_info.is_server) {
    hpw_info("server loop start\n");

    while (true) {
      mgr.update();

      if (mgr.status().has_packets) {
        for (crauto packet: mgr.unload_all()) {
          std::stringstream txt;
          txt << "received packet info:\n";
          txt << "- source ip v4: " << packet.ip_v4 << "\n";
          txt << "- source port: " << packet.port << "\n";
          txt << "- packet size: " << packet.bytes.size() << "\n";
          assert(packet.bytes.size() == 1);
          txt << "- packet byte: " << +packet.bytes[0] << "\n";
          hpw_info(txt.str());
        }
        break;
      }
    }

    hpw_info("total received packets: " + n2s(mgr.status().received_packets) + "\n");
    hpw_info("server loop end\n");
  } else { // client
    net::Packet pck;
    pck.bytes.push_back(99);
    hpw_log("send byte to ip v4 " + _connection_info.target_ip_v4 + ":" + n2s(_connection_info.target_udp_port) + "\n");
    net::Packet_mgr::Target_info target;
    target.ip_v4 = _connection_info.target_ip_v4;
    target.port = _connection_info.target_udp_port;
    target.udp_mode = true;
    mgr.send(pck, target);
    mgr.update();
  }
}

void test_2(net::Packet_mgr& mgr) {
  hpw_info("test 2 - TCP sending byte to server\n");

  if (_connection_info.is_server) {
    hpw_info("waiting for TCP connection...\n");
    cauto net_target = mgr.wait_connection();
    _connection_info.target_ip_v4 = net_target.ip_v4;
    _connection_info.target_tcp_port = net_target.port;
    hpw_info("server loop start\n");

    while (true) {
      mgr.update();

      if (mgr.status().has_packets) {
        for (crauto packet: mgr.unload_all()) {
          std::stringstream txt;
          txt << "received packet info:\n";
          txt << "- source ip v4: " << packet.ip_v4 << "\n";
          txt << "- source port: " << packet.port << "\n";
          txt << "- packet size: " << packet.bytes.size() << "\n";
          assert(packet.bytes.size() == 1);
          txt << "- packet byte: " << +packet.bytes[0] << "\n";
          hpw_info(txt.str());
        }
        break;
      }
    }

    hpw_info("total received packets: " + n2s(mgr.status().received_packets) + "\n");
    hpw_info("server loop end\n");
  } else { // client
    net::Packet_mgr::Target_info target;
    target.port = _connection_info.target_tcp_port;
    target.ip_v4 = _connection_info.target_ip_v4;
    
    hpw_info("connect to TCP server...\n");
    mgr.connect_to(target);

    net::Packet pck;
    pck.bytes.push_back(88);
    hpw_log("send byte to ip v4 " + target.ip_v4 + ":" + n2s(target.port) + "\n");
    mgr.send(pck, target);
    mgr.update();
  }
}

void test_3(net::Packet_mgr& mgr) {
  hpw_info("test 3 - TCP sending byte to client from server\n");
  assert(mgr.status().connected);

  if (_connection_info.is_server) {
    net::Packet_mgr::Target_info target;
    target.port = _connection_info.target_tcp_port;
    target.ip_v4 = _connection_info.target_ip_v4;

    net::Packet pck;
    pck.bytes.push_back(77);
    hpw_log("send byte to ip v4 " + target.ip_v4 + ":" + n2s(target.port) + "\n");
    mgr.send(pck, target);
    mgr.update();    
  } else { // client
    hpw_info("client loop start\n");

    while (true) {
      mgr.update();

      if (mgr.status().has_packets) {
        for (crauto packet: mgr.unload_all()) {
          std::stringstream txt;
          txt << "received packet info:\n";
          txt << "- source ip v4: " << packet.ip_v4 << "\n";
          txt << "- source port: " << packet.port << "\n";
          txt << "- packet size: " << packet.bytes.size() << "\n";
          assert(packet.bytes.size() == 1);
          txt << "- packet byte: " << +packet.bytes[0] << "\n";
          hpw_info(txt.str());
        }
        break;
      }
    }

    hpw_info("total received packets: " + n2s(mgr.status().received_packets) + "\n");
    hpw_info("client loop end\n");
  }
}

int main(int argc, char** argv) {
  parse_args(argc, argv);
  print_params();

  net::Packet_mgr pck_mgr;
  init_pck_mgr(pck_mgr);
  test_1(pck_mgr);
  test_2(pck_mgr);
  test_3(pck_mgr);
}
