#include <cassert>
#include <algorithm>
#include <cmath>
#include "scene-bgp-select.hpp"
#include "game/core/scenes.hpp"
#include "game/core/bgps.hpp"
#include "game/util/resource-helper.hpp"
#include "game/util/keybits.hpp"
#include "game/util/locale.hpp"
#include "game/menu/advanced-text-menu.hpp"
#include "game/menu/item/text-item.hpp"
#include "game/bgp/bgp.hpp"
#include "game/bgp/bgp-util.hpp"
#include "util/hpw-util.hpp"
#include "util/math/timer.hpp"
#include "graphic/image/image.hpp"

struct Scene_bgp_select::Impl {
  Unique<Advanced_text_menu> _menu {};
  Delta_time _bgp_state {};
  Timer _hide_menu_timer {6}; 
  bool _hide_menu = false; // закрывать окно меню при бездействии
  mutable Shared<bgp::Bgp> _bgp {};

  inline Impl() {
    if (hpw::bgp_for_menu.empty())
      hpw::bgp_for_menu = bgp::random_name();
    _bgp = bgp::make(hpw::bgp_for_menu);

    cauto title = get_locale_str("bgp_select.title");
    Rect rect {15, 10, 300, 200};
    Advanced_text_menu_config atm_config {};
    atm_config.bf_bg = &blend_avr;
    atm_config.bf_border = &blend_avr_max;

    init_unique<Advanced_text_menu>(_menu, title, _get_items(), rect, atm_config);
    _find_bgp(hpw::bgp_for_menu);
  }

  // докрутить выбор в меню до установленного фона
  inline void _find_bgp(cr<Str> name) {
    for (uint id = 0; crauto item: _menu->get_items()) {
      if (item->to_text() == utf8_to_32(name)) {
        _menu->set_cur_item_id(id);
        return;
      }
      ++id;
    }
  }

  inline Menu_items _get_items() const {
    Menu_items items;

    // случайный фон
    items.push_back(
      new_shared<Menu_text_item>(
        get_locale_str("bgp_select.random_bg.title"),
        [this]{
          hpw::bgp_auto_swith = true;
          hpw::bgp_for_menu = bgp::random_name();
          _bgp = bgp::make(hpw::bgp_for_menu);
        },
        []->utf32 { return {}; },
        get_locale_str("bgp_select.random_bg.desc")
      )
    );

    // добавить все фоны
    auto bgps = bgp::all_names();
    std::sort(bgps.begin(), bgps.end());

    for (crauto name: bgps) {
      items.push_back(new_shared<Menu_text_item>(
        utf8_to_32(name),
        [this, bgp_name=name]{
          hpw::bgp_auto_swith = false;
          hpw::bgp_for_menu = bgp_name;
          _bgp = bgp::make(hpw::bgp_for_menu);
        }
      ));
    }

    // выйти
    items.push_back(new_shared<Menu_text_item>(get_locale_str("common.back"), []{ hpw::scene_mgr.back(); }));
    
    return items;
  }

  inline void _update_input(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape))
      hpw::scene_mgr.back();
    
    // при выборе тоже выходить
    if (is_pressed_once(hpw::keycode::enable))
      hpw::scene_mgr.back();

    // если долго не нежимать, то меню скроется с экрана
    if (is_any_key_pressed()) {
      _hide_menu = false;
      _hide_menu_timer.reset();
    } else {
      _hide_menu |= _hide_menu_timer.update(dt);
    }
  }

  inline void _update_menu(const Delta_time dt) {
    assert(_menu);
    _menu->update(dt);

    // любое перемещение в меню вызывает мгновенную смену фона
    if (_menu->moved()) {
      crauto exit_from_menu = _menu->get_items().back(); // последняя кнока - выход из меню, её надо проигнорить
      crauto item = _menu->get_cur_item();
      if (exit_from_menu != item)
        item->enable();
    }
  }

  inline void update(const Delta_time dt) {
    assert(_bgp), _bgp->update(dt);
    _update_input(dt);
    _update_menu(dt);
  }

  inline void draw(Image& dst) const {
    assert(_bgp), _bgp->draw(dst);

    if (!_hide_menu)
      assert(_menu), _menu->draw(dst);
  }
}; // Impl 

Scene_bgp_select::Scene_bgp_select(): _impl {new_unique<Impl>()} {}
Scene_bgp_select::~Scene_bgp_select() {}
void Scene_bgp_select::update(const Delta_time dt) { _impl->update(dt); }
void Scene_bgp_select::draw(Image& dst) const { _impl->draw(dst); }
