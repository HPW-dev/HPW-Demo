#include <cassert>
#include "scene-tools.hpp"
#include "scene-input-delay-test.hpp"
#include "game/core/scenes.hpp"
#include "game/menu/menu.hpp"
#include "game/menu/menu-from-yaml.hpp"
#include "game/util/locale.hpp"
#include "game/util/keybits.hpp"
#include "game/util/resource-helper.hpp"
#include "util/file/file.hpp"
#include "util/file/yaml.hpp"

struct Scene_tools::Impl {
  Unique<Menu> _menu {};

  inline explicit Impl() { init_menu(); }

  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape))
      hpw::scene_mgr.back();
    
    assert(_menu);
    _menu->update(dt);
  }

  inline void draw(Image& dst) const {
    assert(_menu);
    _menu->draw(dst);
  }

  inline void init_menu() {
    _menu = menu_from_yaml(
      Yaml(load_res("resource/menu/tools.yml")),
      Action_table {        
        {"input_delay_test", []{ hpw::scene_mgr.add(new_shared<Scene_input_delay_test>()); }},
      }
    );
  }
}; // Impl

MAKE_SCENE_CLASS_IMPL(Scene_tools)
