#include <cassert>
#include "scene-epge.hpp"
#include "scene-epge-list.hpp"
#include "scene-epge-config.hpp"
#include "game/core/scenes.hpp"
#include "game/core/canvas.hpp"
#include "game/core/epges.hpp"
#include "game/util/keybits.hpp"
#include "game/util/locale.hpp"
#include "game/util/game-util.hpp"
#include "plugin/epge/epge-util.hpp"
#include "game/menu/advanced-text-menu.hpp"
#include "game/menu/item/text-item.hpp"

struct Scene_epge::Impl {
  Unique<Advanced_text_menu> _menu {};
  bool _need_reinit_menu {};

  inline explicit Impl() {
    init_menu();
  }

  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape))
      exit_from_scene();

    // при выборе эффекта менюшку надо дополнить
    if (hpw::scene_mgr->status.came_back || _need_reinit_menu) {
      init_menu();
      _need_reinit_menu = false;
    }

    assert(_menu);
    _menu->update(dt);
  }

  inline void draw(Image& dst) const {
    assert(_menu);
    _menu->draw(dst);

    // TODO рисовать чё-то тестовое на фоне
  }

  inline static void exit_from_scene() {
    assert(hpw::scene_mgr);
    hpw::scene_mgr->back();
    save_epges();
  }

  inline void init_menu() {
    // кнопка добавить новый эффект
    Menu_items menu_items {
      new_shared<Menu_text_item>( get_locale_str("scene.graphic_menu.epge.add_new"),
        []{ hpw::scene_mgr->add(new_shared<Scene_epge_list>()); } ),
    };

    // добавить кнопки для управления отдельными эффектами
    for (crauto epge: hpw::epges) {
      assert(epge);
      cauto epge_name = utf8_to_32(epge->name());
      cauto epge_desc = utf8_to_32(epge->desc());
      cauto epge_ptr = epge.get();

      menu_items.emplace_back( new_shared<Menu_text_item>(
        epge_name,
        [ptr=epge_ptr]{ hpw::scene_mgr->add(new_shared<Scene_epge_config>(ptr)); },
        []{ return utf32{}; },
        epge_desc
      ) );
    }

    // ресет и выход
    menu_items.push_back( new_shared<Menu_text_item>( get_locale_str("common.reset"), [this]{
      hpw::epges.clear();
      _need_reinit_menu = true;
    } ) );
    menu_items.push_back( new_shared<Menu_text_item>( get_locale_str("common.exit"), []{ exit_from_scene(); } ) );

    init_unique(_menu, get_locale_str("scene.graphic_menu.epge.title"),
      menu_items, Rect{0, 0, graphic::width, graphic::height} );
  }
}; // Impl

Scene_epge::Scene_epge(): impl {new_unique<Impl>()} {}
Scene_epge::~Scene_epge() {}
void Scene_epge::update(const Delta_time dt) { impl->update(dt); }
void Scene_epge::draw(Image& dst) const { impl->draw(dst); }