#include <cassert>
#include "scene-epge-list.hpp"
#include "game/core/scenes.hpp"
#include "game/core/epges.hpp"
#include "game/util/keybits.hpp"
#include "game/util/locale.hpp"
#include "game/util/game-util.hpp"
#include "game/menu/advanced-text-menu.hpp"
#include "game/menu/item/text-item.hpp"
#include "plugin/epge/epge-util.hpp"

struct Scene_epge_list::Impl {
  Unique<Advanced_text_menu> _menu {};

  inline explicit Impl() {
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

    // накидать названий эффектов в меню
    cauto epge_list = avaliable_epges();
    for (crauto name: epge_list) {
      cauto epge = make_epge(name);
      assert(epge);
      cauto desc = epge->desc();

      menu_items.push_back( new_shared<Menu_text_item>(
        utf8_to_32(name),
        [_name=name]{ hpw::epges.emplace_back(make_epge(_name)); },
        []{ return utf32{}; },
        utf8_to_32(desc)
      ) );
    } // for epge_list

    // Exit item
    menu_items.emplace_back(new_shared<Menu_text_item>( get_locale_str("common.exit"), []{ exit_from_scene(); } ));

    init_unique(_menu, get_locale_str("scene.graphic_menu.epge.list_title"), menu_items, Rect{30, 10, 350, 300} );
  }
}; // Impl

Scene_epge_list::Scene_epge_list(): impl {new_unique<Impl>()} {}
Scene_epge_list::~Scene_epge_list() {}
void Scene_epge_list::update(const Delta_time dt) { impl->update(dt); }
void Scene_epge_list::draw(Image& dst) const { impl->draw(dst); }
