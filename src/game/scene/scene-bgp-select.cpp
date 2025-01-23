#include <cassert>
#include <cmath>
#include "scene-bgp-select.hpp"
#include "game/core/scenes.hpp"
#include "game/util/resource-helper.hpp"
#include "game/util/keybits.hpp"
#include "game/util/locale.hpp"
#include "game/menu/advanced-text-menu.hpp"
#include "game/menu/item/text-item.hpp"
#include "game/bgp/bgp.hpp"
#include "util/hpw-util.hpp"
#include "graphic/image/image.hpp"

struct Scene_bgp_select::Impl {
  Unique<Advanced_text_menu> _menu {};
  Delta_time _bgp_state {};

  inline Impl() {
    if (!hpw::menu_bgp)
      randomize_menu_bgp();

    cauto title = get_locale_str("bgp_select.title");
    Rect rect {15, 10, 300, 200};
    Advanced_text_menu_config atm_config {};
    atm_config.bf_bg = &blend_avr;
    atm_config.bf_border = &blend_avr_max;

    Menu_items items {};
    items.push_back(new_shared<Menu_text_item>(get_locale_str("common.back"), []{ hpw::scene_mgr.back(); }));

    init_unique<Advanced_text_menu>(_menu, title, items, rect, atm_config);
  }

  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape))
      hpw::scene_mgr.back();

    _bgp_state += dt;

    assert(_menu);
    _menu->update(dt);
  }

  inline void draw(Image& dst) const {
    dst.fill(Pal8::red);

    assert(hpw::menu_bgp);
    hpw::menu_bgp(dst, std::floor(pps(_bgp_state)));

    assert(_menu);
    _menu->draw(dst);
  }
}; // Impl 

Scene_bgp_select::Scene_bgp_select(): _impl {new_unique<Impl>()} {}
Scene_bgp_select::~Scene_bgp_select() {}
void Scene_bgp_select::update(const Delta_time dt) { _impl->update(dt); }
void Scene_bgp_select::draw(Image& dst) const { _impl->draw(dst); }
