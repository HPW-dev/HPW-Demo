#include "game/core/canvas.hpp"
#include "host/glfw3/host-glfw.hpp"
#include "host/host-util.hpp"
#include "game/core/locales.hpp"
#include "game/core/common.hpp"
#include "game/core/core.hpp"
#include "game/core/graphic.hpp"
#include "game/menu/advanced-text-menu.hpp"
#include "game/menu/item/int-item.hpp"
#include "game/menu/item/bool-item.hpp"
#include "game/menu/item/text-item.hpp"
#include "game/util/sync.hpp"
#include "game/util/locale.hpp"
#include "game/util/font-helper.hpp"
#include "game/util/keybits.hpp"
#include "util/log.hpp"
#include "util/str-util.hpp"

class Test_app final: public Host_glfw {
public:
  inline explicit Test_app(int argc, char *argv[]): Host_glfw(argc, argv) {}

protected:
  void draw_game_frame() const override {
    rauto dst = *graphic::canvas;

    dst.fill(Pal8::from_real(1.0 / 3.0, true));
    assert(_menu);
    _menu->draw(dst);

    Host_glfw::draw_game_frame();
  }

private:
  void init() override {
    Host_glfw::init();
    load_locale(hpw::locale_path);
    load_fonts();
    set_target_ups(_tickrate);
    graphic::frame_skip = 0;
    graphic::auto_frame_skip = false;
    graphic::set_vsync(false);
    graphic::set_disable_frame_limit(false);
    graphic::set_target_fps(_fps);
    init_menu();
  }

  void update(const Delta_time dt) override {
    if (is_pressed_once(hpw::keycode::escape))
      hpw::soft_exit();

    assert(_menu);
    _menu->update(dt);
  }

  inline void init_menu() {
    const utf32 title = U"Тест плавного движения";
    const Menu_items items {
      new_shared<Menu_int_item>(U"FPS", [this]{ return _fps; }, [this](int val){
        _fps = std::clamp(val, 5, 240);
        graphic::set_target_fps(_fps);
      }, 5),
      new_shared<Menu_int_item>(U"tickrate", [this]{ return _tickrate; }, [this](int val){
        _tickrate = std::clamp(val, 5, 360);
        set_target_ups(_tickrate);
      }, 5),
      new_shared<Menu_text_item>(get_locale_str("common.back"), []{ hpw::soft_exit(); }),
    };
    const Rect rect(Vec{15, 10}, Vec{220, 130});
    Advanced_text_menu_config config;
    config.without_desc = true;
    config.bf_bg = &blend_158;
    config.bf_border = &blend_avr_max;
    init_unique(_menu, title, items, rect, config);
  }

  bool _clamp_alpha {true};
  bool _use_interp {true};
  bool _future_interp {false};
  int _fps {60};
  int _tickrate {30};
  Unique<Advanced_text_menu> _menu {};
}; // Test_app

int main(int argc, char *argv[]) {
  Test_app app(argc, argv);
  app.run();
}
