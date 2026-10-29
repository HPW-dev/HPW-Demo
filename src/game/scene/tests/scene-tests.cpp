#include "pch.hpp"
#include "scene-tests.hpp"

#include "game/scene/common/scene-empty.hpp"
#include "game/scene/tests/scene-interpolation-test.hpp"
#include "game/scene/tests/Scene-qr-generator-test.hpp"
#include "game/scene/tests/scene-input-delay-test.hpp"
#include "game/scene/tests/scene-subpixel-test.hpp"
#include "game/scene/tests/scene-random-test.hpp"
#include "game/util/resource-helper.hpp"
#include "game/util/locale.hpp"
#include "game/menu/menu-from-json.hpp"
#include "game/menu/menu.hpp"
#include "game/core/scenes.hpp"
#include "game/input/keys.hpp"
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
        {"scene_empty", []{ hpw::scene_mgr.add(new_shared<Scene_empty>()); }},
        {"input_delay_test", []{ hpw::scene_mgr.add(new_shared<Scene_input_delay_test>()); }},
        {"qr_generator_test", []{ hpw::scene_mgr.add(new_shared<Scene_qr_generator_test>()); }},
        {"subpixel_test", []{ hpw::scene_mgr.add(new_shared<Scene_subpixel_test>()); }},
        {"interpolation_test", []{ hpw::scene_mgr.add(new_shared<Scene_interpolation_test>()); }},
        {"random_test", []{ hpw::scene_mgr.add(new_shared<Scene_random_test>()); }},
      }
    );
  }
}; // Impl

MAKE_SCENE_CLASS_IMPL(Scene_tests)
