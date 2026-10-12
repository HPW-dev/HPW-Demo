#include "pch.hpp"
#include "scene-tests.hpp"
#include "Scene-qr-generator-test.hpp"
#include "scene-input-delay-test.hpp"
#include "scene-subpixel-test.hpp"
#include "scene-interpolation-test.hpp"
#include "game/core/scenes.hpp"
#include "game/menu/menu.hpp"
#include "game/menu/menu-from-json.hpp"
#include "game/util/locale.hpp"
#include "game/input/keys.hpp"
#include "game/util/resource-helper.hpp"
#include "util/file/file.hpp"

struct Scene_tests::Impl {
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
    _menu = menu_from_json(
      json_from_res("scripts/ui/menu/tests.json"),
      Action_table {        
        {"input_delay_test", []{ hpw::scene_mgr.add(new_shared<Scene_input_delay_test>()); }},
        {"qr_generator_test", []{ hpw::scene_mgr.add(new_shared<Scene_qr_generator_test>()); }},
        {"subpixel_test", []{ hpw::scene_mgr.add(new_shared<Scene_subpixel_test>()); }},
        {"interpolation_test", []{ hpw::scene_mgr.add(new_shared<Scene_interpolation_test>()); }},
      }
    );
  }
}; // Impl

MAKE_SCENE_CLASS_IMPL(Scene_tests)
