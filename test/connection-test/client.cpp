#include <cassert>
#include "client.hpp"
#include "game/core/scenes.hpp"
#include "game/menu/text-menu.hpp"
#include "game/menu/item/text-item.hpp"
#include "game/util/keybits.hpp"
#include "graphic/image/image.hpp"
#include "game/util/locale.hpp"

struct Client::Impl {
  Unique<Menu> _menu {};

  inline Impl() {
    _menu = new_unique<Text_menu>(
      Menu_items {
        new_shared<Menu_text_item>(get_locale_str("common.exit"), []{ hpw::scene_mgr.back(); }),
      },
      Vec{15, 10}
    );
  }

  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape))
      hpw::scene_mgr.back();
    _menu->update(dt);
  }

  inline void draw(Image& dst) const {
    dst.fill(Pal8::black);
    _menu->draw(dst);
  }
}; // Impl 

Client::Client(): _impl {new_unique<Impl>()} {}
Client::~Client() {}
void Client::update(const Delta_time dt) { _impl->update(dt); }
void Client::draw(Image& dst) const { _impl->draw(dst); }
