#include <cassert>
#include "scene-options.hpp"
#include "scene-input.hpp"
#include "scene-graphic.hpp"
#include "scene-game-options.hpp"
#include "scene-hud-select.hpp"
#include "scene-bgp-select.hpp"
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

  inline Impl() { init_menu(); }

  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape))
      hpw::scene_mgr.back();

    assert(_menu);
    _menu->update(dt);

    // чтобы перезагрузить локализацию строк
  if (hpw::scene_mgr.status().came_back)
    init_menu();
  }

  inline void draw(Image& dst) const {
    dst.fill(Pal8::black);

    assert(_menu);
    _menu->draw(dst);
  }

  inline void init_menu() {
    cauto config_file = load_res("resource/menu/options.yml");
    Yaml config(config_file);
    _menu = menu_from_yaml(
      config,
      Action_table {        
        {"graphic_opts", Action_container( Menu_item::Action([]{ hpw::scene_mgr.add(new_shared<Scene_graphic>()); }) )},
        {"hud_opts", Action_container( Menu_item::Action([]{ hpw::scene_mgr.add(new_shared<Scene_hud_select>()); }) )},
        {"input_opts", Action_container( Menu_item::Action([]{ hpw::scene_mgr.add(new_shared<Scene_input>()); }) )},
        {"game_opts", Action_container( Menu_item::Action([]{ hpw::scene_mgr.add(new_shared<Scene_game_options>()); }) )},
        {"bgp_select", Action_container( Menu_item::Action([]{ hpw::scene_mgr.add(new_shared<Scene_bgp_select>()); }) )},
      }
    );
  }
}; // Impl 

Scene_options::Scene_options(): _impl {new_unique<Impl>()} {}
Scene_options::~Scene_options() {}
void Scene_options::update(const Delta_time dt) { _impl->update(dt); }
void Scene_options::draw(Image& dst) const { _impl->draw(dst); }
