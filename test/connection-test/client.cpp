#include <cassert>
#include "client.hpp"
#include "test-packets.hpp"
#include "game/scene/msgbox/msgbox-enter.hpp"
#include "game/scene/scene-enter-text.hpp"
#include "game/core/scenes.hpp"
#include "game/core/fonts.hpp"
#include "game/core/user.hpp"
#include "game/menu/text-menu.hpp"
#include "game/menu/item/text-item.hpp"
#include "game/util/keybits.hpp"
#include "game/util/locale.hpp"
#include "graphic/image/image.hpp"
#include "util/net/udp-packet-mgr.hpp"
#include "util/log.hpp"
#include "util/error.hpp"
#include "util/str-util.hpp"
#include "util/math/random.hpp"

struct Client::Impl {
  constx uint CONNECTION_INTERVAL = 240 * 1.5;
  uint _reserve_connection_timer {CONNECTION_INTERVAL}; // таймаут резервной отправки сигнала о коннекте
  Unique<Menu> _menu {};
  Str _server_ipv4 {};
  utf32 _server_name {};
  decltype(Packet_server_info::connected_players) _server_players {};
  Delta_time _server_ping {-1};
  net::Udp_packet_mgr _upm {};
  uint _total_loaded_packets {};

  inline explicit Impl(cr<Str> ip_v4, const net::Port port) {
    _menu = new_unique<Text_menu>(
      Menu_items {
        new_shared<Menu_text_item>(U"попытаться подключиться", [this]{ try_to_connect(); }),
        new_shared<Menu_text_item>(U"подключиться по IPv4", [this]{
          hpw::scene_mgr.add(new_shared<Scene_enter_text>(
            U"Введите IP сервера",
            U"Enter для подтверждения, Escape чтобы выйти",
            U"Server IPv4: ",
            utf8_to_32(_server_ipv4),
            [this](cr<utf32> text) {            
              try {
                _server_ipv4 = utf32_to_8(text);
                _upm.send(get_connection_packet(), _server_ipv4);
              }  catch (cr<hpw::Error> err) {
                hpw::scene_mgr.add( new_shared<Scene_msgbox_enter>(U"ошибка при подключении к серверу: " +
                  utf8_to_32(err.what()), get_locale_str("common.warning")) );
                hpw::scene_mgr.back();
              } catch (...) {
                hpw::scene_mgr.add( new_shared<Scene_msgbox_enter>(U"неизвестная ошибка при подключении к серверу",
                  get_locale_str("common.warning")) );
                hpw::scene_mgr.back();
              }
            }
          ));
        }), // кнопка подключиться по ipv4
        new_shared<Menu_text_item>(get_locale_str("common.exit"), []{ hpw::scene_mgr.back(); }),
      },
      Vec{15, 10}
    );

    if (hpw::player_name.empty()) {
      hpw::player_name = U" ʕ•ᴥ•ʔ Тестовый игрок (";
      cauto rnd_num = rndu_fast(999);
      hpw::player_name += n2s<utf32>(rnd_num);
      hpw::player_name += U")";
    }
    
    try {
      _upm.start_client(ip_v4, port);
    } catch (cr<hpw::Error> err) {
      hpw::scene_mgr.add( new_shared<Scene_msgbox_enter>(U"ошибка при создании создании клиента: " +
        utf8_to_32(err.what()), get_locale_str("common.warning")) );
      hpw::scene_mgr.back();
    } catch (...) {
      hpw::scene_mgr.add( new_shared<Scene_msgbox_enter>(U"неизвестная ошибка при создании клиента",
        get_locale_str("common.warning")) );
      hpw::scene_mgr.back();
    }
  }

  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape))
      hpw::scene_mgr.back();

    _menu->update(dt);

    if (_upm.is_active()) {
      try {
        _upm.update();
        process_packets();

        if (!_server_ipv4.empty() && --_reserve_connection_timer == 0) {
          _reserve_connection_timer = CONNECTION_INTERVAL;
          send_connection();
        }
      } catch (cr<hpw::Error> err) {
        hpw::scene_mgr.add( new_shared<Scene_msgbox_enter>(U"ошибка обмена данных" +
          utf8_to_32(err.what()), get_locale_str("common.warning")) );
        hpw::scene_mgr.back();
      } catch (...) {
        hpw::scene_mgr.add( new_shared<Scene_msgbox_enter>(U"неизвестная ошибка обмена данных",
          get_locale_str("common.warning")) );
        hpw::scene_mgr.back();
      }
    }
  }

  inline void draw(Image& dst) const {
    dst.fill(Pal8::from_real(0.171717, true));
    _menu->draw(dst);
    draw_server_info(dst);
  }

  inline void try_to_connect() {
    if (_server_ipv4.empty()) {
      hpw_log("нету адреса для подключения\n");
      return;
    }

    hpw_log("попытка подключения к серверу: \"" + _server_ipv4 + "\"\n");
    send_connection();
  }

  inline void draw_server_info(Image& dst) const {
    crauto font = graphic::font;
    assert(font);
    utf32 text;
    text += U"player name: " + hpw::player_name + U"\n";
    text += U"loaded packets: " + n2s<utf32>(_total_loaded_packets) + U"\n";
    text += U"\n";
    text += U"S E R V E R\n";
    text += U"* IPv4 " + (_server_ipv4.empty() ? U"-" : utf8_to_32(_server_ipv4)) + U"\n";
    text += U"* Name " + (_server_name.empty() ? U"-" : _server_name) + U"\n";
    text += U"* Players " + (_server_ipv4.empty() ? U"-" : n2s<utf32>(_server_players)) + U"\n";
    text += U"* Ping " + (_server_ping < 0 ? U"-" : (n2s<utf32>(_server_ping) + U" ms.")) + U"\n";
    const Vec pos(50, 60);
    font->draw(dst, pos, text);
  }

  inline net::Packet get_connection_packet() const {
    net::Packet packet = new_packet<Packet_connect>();
    rauto raw = net::bytes_to_packet<Packet_connect>(packet.bytes);
    prepare_game_version(raw.game_version);
    prepare_short_nickname(raw.short_nickname, SHORT_NICKNAME_SZ);
    raw.hash = net::get_hash(packet);
    hpw_log("connection packet created for \"" + _server_ipv4 + "\" (hash: " + n2hex(raw.hash) + ")\n");
    return packet;
  }

  inline void send_connection() {
    _upm.push(get_connection_packet(), _server_ipv4);
  }

  // разбор полученных пакетов
  inline void process_packets() {
    return_if(!_upm.has_packets());
    cauto packets = _upm.unload_all();

    for (crauto packet: packets) {
      ++_total_loaded_packets;

      // пустые пакеты игнорить
      if (packet.bytes.size() < sizeof(Tag)) {
        hpw_log("packet data is small, ignore\n");
        continue;
      }

      // при несовпадении контрольной суммы игнор
      cauto local_hash = net::get_hash(packet);
      if (local_hash != net::find_packet_hash(packet)) {
        hpw_log("packet hash is not equal, ignore\n");
        continue;
      }

      cauto tag = find_packet_tag(packet);

      switch (tag) {
        case Tag::ERROR: error("tag error"); break;
        case Tag::EMPTY: hpw_log("empty tag, ignore\n"); break;
        case Tag::SERVER_BROADCAST: process_broadcast(packet); break;
        case Tag::CLIENT_CONNECT: hpw_log("client connect, ignore\n"); break;
        case Tag::DISCONNECT: process_disconnect(packet); break;
        default: hpw_log("unknown tag, ignore\n"); break;
      }
    } // for packets
  }

  inline void process_disconnect(cr<net::Packet> src) {
    hpw_log("process disconnect packet...\n");

    if (src.bytes.size() != sizeof(Packet_disconnect)) {
      hpw_log("размер пакета несовпадает с Packet_disconnect, игнор\n");
      return;
    }

    crauto raw = net::bytes_to_packet<Packet_disconnect>(src.bytes);
    // если сервер отключает тебя:
    if (raw.disconnect_you) {
      error("need impl");
    } else { // если сервер отключился сам:
      _server_players = {};
      _server_ipv4.clear();
      _server_name.clear();
      _server_ping = -1;
    }
  }

  inline void process_broadcast(cr<net::Packet> packet) {
    hpw_log("process broadcast packet...\n");

    if (packet.bytes.size() != sizeof(Packet_server_info)) {
      hpw_log("размер пакета несовпадает с Packet_server_info, игнор\n");
      return;
    }

    _server_ipv4 = packet.ip_v4;
    crauto pb = net::bytes_to_packet<Packet_server_info>(packet.bytes);
    _server_players = pb.connected_players;
    _server_name = pb.short_nickname;

    // check ver:
    Version local_ver;
    prepare_game_version(local_ver);
    hpw_log(Str("версии игр ") + (local_ver == pb.game_version ? "совпадают" : "не совпадают") + "\n");
  }
}; // Impl 

Client::Client(cr<Str> ip_v4, const net::Port port): _impl {new_unique<Impl>(ip_v4, port)} {}
Client::~Client() {}
void Client::update(const Delta_time dt) { _impl->update(dt); }
void Client::draw(Image& dst) const { _impl->draw(dst); }
