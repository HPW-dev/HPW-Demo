#include "pch.hpp"
#include "scene-epiwarn.hpp"

#include "engine/graphic/util/graphic-util.hpp"
#include "game/util/sprite-helper.hpp"
#include "game/util/version.hpp"
#include "game/core/graphic.hpp"
#include "game/core/scenes.hpp"
#include "game/core/common.hpp"
#include "game/core/canvas.hpp"
#include "game/core/fonts.hpp"
#include "game/input/keys.hpp"

// интерполятор для плавного нарастания яркости
staticx inline real ease_in_out_cubic(real t) {
  return t < 0.5
    ? 4.0 * t * t * t
    : 1.0 - std::pow(-2.0 * t + 2.0, 3.0) * 0.5;
}

struct Scene_epiwarn::Impl {
  cr<Image> _warn_scr;
  real _brightness {};
  bool _show_text {false}; // показывает табличку о том, чё надо нажать для скипа
  bool _use_delay {true};
  bool _skip_me {false};
  bool _border_bkp;
  Timer _text_delay;
  Timer _delay;

  Impl()
  : _warn_scr{find_image_safe("graphic/images/misc/epilepsy-warning.png")}
  , _delay(6)
  , _text_delay(8.5)
  {
    // пока рамку вырубить
    _border_bkp = graphic::draw_border;
    graphic::draw_border = false;

    // при первом запуске не пропускать табличку, мало ли
    if (!hpw::first_start) {
      next_scene();
      return;
    }

    // В отладке пропускаем этот экран
    cauto builde_preset = Str(get_game_build_preset());
    if (builde_preset == "fast-build"
    ||  builde_preset == "fast-build-x32"
    ||  builde_preset == "fast-build-x64"
    ||  builde_preset == "debug"
    ||  builde_preset == "debug-x32"
    ||  builde_preset == "debug-x64"
    ) {
      next_scene();
      return;
    }
  }

  void update(const Delta_time dt) {
    if (_skip_me)
      hpw::scene_mgr.back();

    check_input();

    if (_use_delay) {
      if (_delay.update(dt))
        _use_delay = false;

      _brightness = ease_in_out_cubic(_delay.ratio());
    } else {
      _brightness = 0;
    }
    
    _show_text |= _text_delay.update(dt);
  }

  void check_input() {
    if (is_pressed_once(hpw::keycode::escape))
      hpw::scene_mgr.back();

    if (is_pressed_once(hpw::keycode::enable))
      next_scene();
  }

  void next_scene() {
    _skip_me = true;
    graphic::draw_border = _border_bkp;
    hpw::scene_mgr.add("Scene_main_menu");

    // спросить о языке при первом запуске
    if (hpw::first_start) {
      // набрать никнейм
      hpw::scene_mgr.add("Scene_nickname");
      // выбрать язык
      hpw::scene_mgr.add("Scene_locale_select");
    }
  }

  void draw(Image& dst) const {
    insert_fast(dst, _warn_scr);
    apply_brightness(dst, _brightness * -255);
    if (_show_text) draw_text(dst);
  }

  void draw_text(Image& dst) const {
    cauto TEXT = U"> " + get_key_name(hpw::keycode::enable) + U" <";
    cauto TEXT_WIDTH = graphic::font->text_width(TEXT);
    const Veci TEXT_POS((dst.X - TEXT_WIDTH) / 2, dst.Y - 108/2);
    graphic::font->draw(dst, TEXT_POS, TEXT, &blend_alpha, 255-204);
  }
}; // Impl

MAKE_SCENE_CLASS_IMPL(Scene_epiwarn)
