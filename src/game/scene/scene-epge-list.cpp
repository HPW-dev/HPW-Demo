#include <cassert>
#include "scene-epge-list.hpp"
#include "scene-epge-config.hpp"
#include "game/core/scenes.hpp"
#include "game/core/epges.hpp"
#include "game/util/keybits.hpp"
#include "game/util/locale.hpp"
#include "game/util/game-util.hpp"
#include "game/menu/advanced-text-menu.hpp"
#include "game/menu/item/text-item.hpp"
#include "game/menu/menu-util.hpp"
#include "plugin/epge/epge-util.hpp"

struct Scene_epge_list::Impl {
  Unique<Advanced_text_menu> _menu {};
  bool _need_bottom_item {}; // переходит на нижний пункт меню

  inline explicit Impl() {
    init_menu();
  }

  inline void update(const Delta_time dt) {
    assert(_menu);

    if (is_pressed_once(hpw::keycode::escape))
      exit_from_scene();

    if (_need_bottom_item) {
      _menu->next_item();
      _need_bottom_item = false;
    }

    _menu->update(dt);
  }

  inline void draw(Image& dst) const {
    assert(_menu);
    _menu->draw(dst);

    // TODO выбранный EPGE так же влияет на фон под меню 
  }

  inline static void exit_from_scene() {
    hpw::scene_mgr.back();
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
        [_name=name]{
          hpw::scene_mgr.back();
          auto epge = make_epge(_name);
          assert(epge);
          hpw::scene_mgr.add( new_shared<Scene_epge_config>(epge.get()) );
          hpw::epges.emplace_back(std::move(epge));
        },
        []{ return utf32{}; },
        utf8_to_32(desc)
      ) );
    } // for epge_list

    
    menu_items.push_back( make_menu_separator(&_need_bottom_item) );
    // Exit item
    menu_items.emplace_back(new_shared<Menu_text_item>( get_locale_str("common.exit"), []{ exit_from_scene(); } ));

    init_unique(_menu, get_locale_str("scene.graphic_menu.epge.list_title"), menu_items, Rect{30, 10, 350, 300} );
  }
}; // Impl

Scene_epge_list::Scene_epge_list(): impl {new_unique<Impl>()} {}
Scene_epge_list::~Scene_epge_list() {}
void Scene_epge_list::update(const Delta_time dt) { impl->update(dt); }
void Scene_epge_list::draw(Image& dst) const { impl->draw(dst); }
