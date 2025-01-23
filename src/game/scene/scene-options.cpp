/*
#include "scene-options.hpp"
#include "scene-mgr.hpp"
#include "scene-input.hpp"
#include "scene-graphic.hpp"
#include "scene-game-options.hpp"
#include "scene-hud-select.hpp"
#include "scene-locale.hpp"
#include "game/core/fonts.hpp"
#include "game/core/scenes.hpp"
#include "game/util/keybits.hpp"
#include "game/util/locale.hpp"
#include "game/util/blur-helper.hpp"
#include "game/menu/text-menu.hpp"
#include "game/menu/item/bool-item.hpp"
#include "game/menu/item/text-item.hpp"
#include "graphic/image/image.hpp"

Scene_options::Scene_options() {
  init_menu();
}

void Scene_options::update(const Delta_time dt) {
  if (is_pressed_once(hpw::keycode::escape))
    hpw::scene_mgr.back();
  menu->update(dt);

  // чтобы перезагрузить локализацию строк
  if (hpw::scene_mgr.status.came_back)
    init_menu();
}

void Scene_options::draw(Image& dst) const {
  dst.fill(Pal8::black);
  menu->draw(dst);
  graphic::font->draw(dst, {20, 30}, get_locale_str("options.title"), &blend_max);
}

void Scene_options::init_menu() {
  init_shared<Text_menu>( menu,
    Menu_items {
      new_shared<Menu_text_item>(get_locale_str("options.graphic"), []{ hpw::scene_mgr.add(new_shared<Scene_graphic>()); }),
      new_shared<Menu_text_item>(get_locale_str("hud_select.title"), []{ hpw::scene_mgr.add(new_shared<Scene_hud_select>()); }),
      // TODO new_shared<Menu_text_item>(get_locale_str("options.sound"), []{ hpw_log("need impl. for Sound settings\n"); }),
      new_shared<Menu_text_item>(get_locale_str("options.input"), []{ hpw::scene_mgr.add(new_shared<Scene_input>()); }),
      new_shared<Menu_text_item>(get_locale_str("game_opts.title"), []{ hpw::scene_mgr.add(new_shared<Scene_game_options>()); }),
      new_shared<Menu_text_item>(hpw::locale_select_title, []{ hpw::scene_mgr.add(new_shared<Scene_locale_select>()); }),
      new_shared<Menu_text_item>(get_locale_str("common.exit"), []{ hpw::scene_mgr.back(); }),
    },
    Vec{60, 80}
  );
} // init_menu
*/

#include <cassert>
#include "scene-options.hpp"
#include "scene-input.hpp"
#include "scene-graphic.hpp"
#include "scene-game-options.hpp"
#include "scene-hud-select.hpp"
#include "game/menu/menu-from-yaml.hpp"
#include "game/menu/item/item.hpp"
#include "game/core/scenes.hpp"
#include "game/util/resource-helper.hpp"
#include "game/util/keybits.hpp"
#include "util/file/file.hpp"
#include "util/file/yaml.hpp"
#include "graphic/image/image.hpp"

struct Scene_options::Impl {
  Unique<Menu> _menu {};

  inline Impl() {
    cauto config_file = load_res("resource/menu/options.yml");
    Yaml config(config_file);
    _menu = menu_from_yaml(
      config,
      Action_table {        
        {"graphic_opts", Action_container( Menu_item::Action([]{ hpw::scene_mgr.add(new_shared<Scene_graphic>()); }) )},
        {"hud_opts", Action_container( Menu_item::Action([]{ hpw::scene_mgr.add(new_shared<Scene_hud_select>()); }) )},
        {"input_opts", Action_container( Menu_item::Action([]{ hpw::scene_mgr.add(new_shared<Scene_input>()); }) )},
        {"game_opts", Action_container( Menu_item::Action([]{ hpw::scene_mgr.add(new_shared<Scene_game_options>()); }) )},
      }
    );
  }

  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape))
      hpw::scene_mgr.back();

    assert(_menu);
    _menu->update(dt);
  }

  inline void draw(Image& dst) const {
    dst.fill(Pal8::black);

    assert(_menu);
    _menu->draw(dst);
  }
}; // Impl 

Scene_options::Scene_options(): _impl {new_unique<Impl>()} {}
Scene_options::~Scene_options() {}
void Scene_options::update(const Delta_time dt) { _impl->update(dt); }
void Scene_options::draw(Image& dst) const { _impl->draw(dst); }
