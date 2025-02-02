#include "host/glfw3/host-glfw.hpp"
#include "host/host-util.hpp"
#include "game/core/canvas.hpp"
#include "game/core/sprites.hpp"
#include "game/core/locales.hpp"
#include "game/core/common.hpp"
#include "game/core/core.hpp"
#include "game/core/graphic.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/util/graphic-util.hpp"
#include "game/menu/advanced-text-menu.hpp"
#include "game/menu/item/int-item.hpp"
#include "game/menu/item/bool-item.hpp"
#include "game/menu/item/text-item.hpp"
#include "game/util/sync.hpp"
#include "game/util/locale.hpp"
#include "game/util/font-helper.hpp"
#include "game/util/keybits.hpp"
#include "util/log.hpp"
#include "util/hpw-util.hpp"
#include "util/str-util.hpp"

constexpr real BORDER_TOP = 40;
constexpr real BORDER_BOTTOM = 384 - BORDER_TOP;

static bool _clamp_alpha {true};
static bool _use_interp {true};

struct Object {
  Vec pos {};
  Vec vel {};
  mutable Vec old_draw_pos {};
  mutable Vec cur_draw_pos {};

  Object() = default;
  ~Object() = default;

  inline void draw(Image& dst) const {
    crauto spr = hpw::sprites.find("object");
    assert(spr);
    const auto spr_center = Vec(spr->X(), spr->Y()) / 2.0;
    old_draw_pos = cur_draw_pos;
    cur_draw_pos = pos - spr_center;
    
    auto draw_pos = cur_draw_pos;
    if (_use_interp) {
      auto alpha = graphic::lerp_alpha;
      if (_clamp_alpha)
        alpha = std::clamp<real>(alpha, 0, 1);
      draw_pos.x = std::lerp<real>(old_draw_pos.x, cur_draw_pos.x, alpha);
      draw_pos.y = std::lerp<real>(old_draw_pos.y, cur_draw_pos.y, alpha);
    }
    cur_draw_pos = draw_pos;
    
    insert(dst, *spr, draw_pos);
  }

  inline void update(Delta_time dt) {
    pos += vel * dt;
    
    fut_pos = pos;
    fut_pos += vel * dt;

    if (pos.y < BORDER_TOP) {
      pos.y = BORDER_TOP;
      vel.y *= -1;
    }
    if (pos.y >= BORDER_BOTTOM) {
      pos.y = BORDER_BOTTOM - 1;
      vel.y *= -1;
    }
  }
}; // Object

using Objects = Vector<Object>;

class Test_app final: public Host_glfw {
public:
  inline explicit Test_app(int argc, char *argv[]): Host_glfw(argc, argv) {}

protected:
  inline void draw_game_frame() const override {
    hpw::soft_draw_start_time = get_time();
    graphic::lerp_alpha = std::clamp<Delta_time>(
      safe_div(hpw::soft_draw_start_time - hpw::tick_end_time, hpw::target_tick_time), 0, 1);

    rauto dst = *graphic::canvas;
    dst.fill(Pal8::black);
    for (crauto obj: _objects)
      obj.draw(dst);
    assert(_menu);
    _menu->draw(dst);

    Host_glfw::draw_game_frame();
  }

private:
  inline void init() override {
    Host_glfw::init();
    load_locale(hpw::locale_path);
    load_fonts();
    set_target_ups(_tickrate);
    graphic::frame_skip = 0;
    graphic::auto_frame_skip = false;
    graphic::set_vsync(false);
    graphic::set_disable_frame_limit(false);
    graphic::set_target_fps(_fps);
    init_objects();
    init_menu();
  }

  inline void update(const Delta_time dt) override {
    if (is_pressed_once(hpw::keycode::escape))
      hpw::soft_exit();

    for (rauto obj: _objects)
      obj.update(dt);

    assert(_menu);
    _menu->update(dt);
    hpw::tick_end_time = get_time();
  }

  inline void init_menu() {
    const utf32 title = U"Тест плавного движения";
    const Menu_items items {
      new_shared<Menu_int_item>(U"FPS", [this]{ return _fps; }, [this](int val){
        _fps = std::clamp(val, 5, 240);
        graphic::set_target_fps(_fps);
      }, 5),
      new_shared<Menu_int_item>(U"тикрейт", [this]{ return _tickrate; }, [this](int val){
        _tickrate = std::clamp(val, 5, 360);
        set_target_ups(_tickrate);
      }, 5),
      new_shared<Menu_bool_item>(U"использовать интерполяцию", []{ return _use_interp; }, [](bool val){ _use_interp = val; }),
      new_shared<Menu_bool_item>(U"лимитировать коэф-т интерп-и", []{ return _clamp_alpha; }, [](bool val){ _clamp_alpha = val; }),
      new_shared<Menu_text_item>(get_locale_str("common.exit"), []{ hpw::soft_exit(); }),
    };
    const Rect rect(Vec{12, 12}, Vec{280, 130});
    Advanced_text_menu_config config;
    config.without_desc = true;
    config.bf_bg = &blend_158;
    config.bf_border = &blend_avr_max;
    init_unique(_menu, title, items, rect, config);
  }

  inline void init_objects() {
    // добавить картинку для объектов
    Shared<Sprite> obj_spr = new_shared<Sprite>(24, 24);
    obj_spr->mask().fill(Pal8::mask_visible);
    obj_spr->image().fill(Pal8::red);
    hpw::sprites.move("object", std::move(obj_spr));

    _objects.resize(8);
    for (int i = 0; rauto obj: _objects) {
      obj.pos = Vec(40 + i * 64, BORDER_TOP);
      obj.vel.y = (i + 1) * 3.0_pps;
      ++i;
    }
  }

  int _fps {60};
  int _tickrate {30};
  Unique<Advanced_text_menu> _menu {};
  Objects _objects {};
}; // Test_app

int main(int argc, char *argv[]) {
  Test_app app(argc, argv);
  app.run();
}
