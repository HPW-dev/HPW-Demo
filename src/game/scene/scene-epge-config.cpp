#include <cassert>
#include "scene-epge-config.hpp"
#include "game/core/canvas.hpp"
#include "game/core/scenes.hpp"
#include "game/util/keybits.hpp"
#include "game/util/locale.hpp"
#include "game/util/game-util.hpp"
#include "plugin/epge/epge.hpp"
#include "game/menu/advanced-text-menu.hpp"
#include "game/menu/item/text-item.hpp"

class Epge_menu_item final: public Menu_item {
  Menu* _menu {};
  epge::Param* _epge_param {};

public:
  inline explicit Epge_menu_item(Menu* menu, epge::Param* epge_param)
  : _menu {menu}
  , _epge_param {epge_param}
  {
    assert(_epge_param);
  }

  inline void enable() override { _epge_param->enable(); }

  inline void plus() override {
    assert(_menu);
    if (_menu->holded())
      _epge_param->plus_value_fast();
    else
      _epge_param->plus_value();
  }

  inline void minus() override {
    assert(_menu);
    if (_menu->holded())
      _epge_param->minus_value_fast();
    else
      _epge_param->minus_value();
  }

  inline utf32 to_text() const override {
    return {}; // TODO
  }

  inline utf32 get_description() const override {
    return {}; // TODO
  }
}; // Epge_menu_item

struct Scene_epge_config::Impl {
  epge::Base* _epge {};
  Unique<Advanced_text_menu> _menu {};

  inline explicit Impl(epge::Base* epge): _epge {epge} {
    assert(_epge);
    init_menu();
  }

  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape))
      exit_from_scene();

    assert(_menu);
    _menu->update(dt);
  }

  inline void draw(Image& dst) const {
    assert(_menu);
    _menu->draw(dst);
  }

  inline static void exit_from_scene() {
    assert(hpw::scene_mgr);
    hpw::scene_mgr->back();
  }

  inline void init_menu() {
    Menu_items menu_items;

    // TODO перечислить настройки эффекта
    for (crauto param: _epge->params())
      menu_items.push_back(new_shared<Epge_menu_item>(_menu.get(), param.get()));

    // exit item
    menu_items.push_back( new_shared<Menu_text_item>( get_locale_str("common.exit"), []{ exit_from_scene(); } ) );

    init_unique(_menu, utf8_to_32(_epge->name()), menu_items, Rect{0, 0, graphic::width, graphic::height} );
  }
}; // Impl

Scene_epge_config::Scene_epge_config(epge::Base* epge): impl {new_unique<Impl>(epge)} {}
Scene_epge_config::~Scene_epge_config() {}
void Scene_epge_config::update(const Delta_time dt) { impl->update(dt); }
void Scene_epge_config::draw(Image& dst) const { impl->draw(dst); }
