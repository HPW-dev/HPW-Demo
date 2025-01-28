#include "server.hpp"
#include "netcode.hpp"
#include "game/core/scenes.hpp"
#include "game/menu/text-menu.hpp"
#include "game/menu/item/text-item.hpp"
#include "game/util/keybits.hpp"
#include "game/util/locale.hpp"
#include "graphic/image/image.hpp"
#include "util/str-util.hpp"

struct Server::Impl {
  Unique<Netcode> _netcode {};
  Unique<Menu> _menu {};

  inline explicit Impl(cr<Connection_ctx> ctx)
  : _netcode {new_unique<Netcode>(true, ctx)}
  {
    _menu = new_unique<Text_menu>(
      Menu_items { new_shared<Menu_text_item>(get_locale_str("common.exit"), []{ hpw::scene_mgr.back(); }), },
      Vec{15, 10}
    );
  }

  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape))
      hpw::scene_mgr.back();

    _menu->update(dt);
    _netcode->update(dt);
  }

  inline void draw(Image& dst) const {
    dst.fill(Pal8::from_real(0.17171717));
    _menu->draw(dst);
    _netcode->draw(dst);
  }
}; // Impl 

Server::Server(cr<Connection_ctx> ctx): _impl {new_unique<Impl>(ctx)} {}
Server::~Server() {}
void Server::update(const Delta_time dt) { _impl->update(dt); }
void Server::draw(Image& dst) const { _impl->draw(dst); }
