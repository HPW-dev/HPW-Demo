#include "util/net/net-packet-mgr.hpp"
#include "util/pparser.hpp"
#include "util/log.hpp"
#include "util/error.hpp"
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
    log_info << "Usage:\n" + arg_parser.get_info();

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
  log_info << txt.str();
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
  log_info << "test 1 - UDP sending byte to server";

  if (_connection_info.is_server) {
    log_info << "server loop start";

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
          log_info << txt.str();
        }
        break;
      }
    }

    log_info << "total received packets: " << mgr.status().received_packets;
    log_info << "server loop end";
  } else { // client
    net::Packet pck;
    pck.bytes.push_back(99);
    log_info << "send byte to ip v4 " << _connection_info.target_ip_v4 << ":" <<
      << _connection_info.target_udp_port;
    net::Packet_mgr::Target_info target;
    target.ip_v4 = _connection_info.target_ip_v4;
    target.port = _connection_info.target_udp_port;
    target.udp_mode = true;
    mgr.send(pck, target);
    mgr.update();
  }
}

void test_2(net::Packet_mgr& mgr) {
  log_info << "test 2 - TCP sending byte to server";

  if (_connection_info.is_server) {
    log_info << "waiting for TCP connection...";
    cauto net_target = mgr.wait_connection();
    _connection_info.target_ip_v4 = net_target.ip_v4;
    _connection_info.target_tcp_port = net_target.port;
    log_info << "server loop start";

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
          log_info << txt.str();
        }
        break;
      }
    }

    log_info << "total received packets: " + n2s(mgr.status().received_packets);
    log_info << "server loop end";
  } else { // client
    net::Packet_mgr::Target_info target;
    target.port = _connection_info.target_tcp_port;
    target.ip_v4 = _connection_info.target_ip_v4;
    
    log_info << "connect to TCP server...";
    mgr.connect_to(target);

    net::Packet pck;
    pck.bytes.push_back(88);
    log_info << "send byte to ip v4 " + target.ip_v4 + ":" << target.port;
    mgr.send(pck, target);
    mgr.update();
  }
}

void test_3(net::Packet_mgr& mgr) {
  log_info << "test 3 - TCP sending byte to client from server";
  assert(mgr.status().connected);

  if (_connection_info.is_server) {
    net::Packet_mgr::Target_info target;
    target.port = _connection_info.target_tcp_port;
    target.ip_v4 = _connection_info.target_ip_v4;

    net::Packet pck;
    pck.bytes.push_back(77);
    log_info << "send byte to ip v4 " + target.ip_v4 + ":" << target.port;
    mgr.send(pck, target);
    mgr.update();    
  } else { // client
    log_info << "client loop start";

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
          log_info << txt.str();
        }
        break;
      }
    }

    log_info << "total received packets: " + n2s(mgr.status().received_packets);
    log_info << "client loop end";
  }
}

void test_4_client(net::Packet_mgr& mgr) {
  log_info << "packet waiting loop...";
  bool test_ran = true;
  mgr.set_receive_cb([] { log_info << "(Cb test) end of receiving"; });

  while (test_ran) {
    mgr.update(); 

    if (mgr.status().has_packets)
      for (crauto packet: mgr.unload_all()) {
        auto sz = packet.bytes.size();
        log_info << "packet size " + n2s(sz);
        log_info << "packet check...";
        cfor (i, sz)
          iferror(scast<byte>(i) != packet.bytes[i], "bad packet data");
        test_ran = false;
      }
  }

  log_info << "end of loop";
  mgr.set_receive_cb({});
}

void test_4_server(net::Packet_mgr& mgr) {
  log_info << "send packet...";

  auto sz = 400;
  log_info << "packet size " << sz;
  net::Packet pck;
  pck.bytes.resize(sz);
  cfor (i, sz)
    pck.bytes[i] = scast<byte>(i);

  net::Packet_mgr::Target_info tgt;
  tgt.async = false;
  tgt.udp_mode = false;
  tgt.ip_v4 = _connection_info.target_ip_v4;
  tgt.port = _connection_info.target_tcp_port;
  tgt.send_cb = [] { log_info << "(Cb test) end of packet sending"; };

  mgr.send(pck, tgt);
  mgr.update(); 
}

void test_4(net::Packet_mgr& mgr) {
  log_info << "test 4 - medium packet send";

  if (_connection_info.is_server)
    test_4_server(mgr);
  else
    test_4_client(mgr);

  log_info << "test 4 - end";
}

int main(int argc, char** argv) {
  parse_args(argc, argv);
  print_params();

  net::Packet_mgr pck_mgr;
  init_pck_mgr(pck_mgr);
  test_1(pck_mgr);
  test_2(pck_mgr);
  test_3(pck_mgr);
  test_4(pck_mgr);
}
