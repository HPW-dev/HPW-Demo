#include "pch.hpp"
#include "scene-version-info.hpp"

#include "engine/graphic/util/util-templ.hpp"
#include "game/util/version.hpp"
#include "game/util/locale.hpp"
#include "game/input/keys.hpp"
#include "game/core/scenes.hpp"
#include "game/core/canvas.hpp"
#include "game/core/fonts.hpp"

struct Scene_version_info::Impl {
  utf32 _msg {};

  inline Impl() {
    _msg += get_locale_str("version_info.game_ver") + U" : "
      + utf8_to_32(get_game_version()) + U"\n";
  }

  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape))
      hpw::scene_mgr.back();
  }

  inline void draw(Image& dst) const {
    dst.fill({});
    graphic::font->draw(dst, {5, 5}, _msg);
  }
}; // Impl

MAKE_SCENE_CLASS_IMPL(Scene_version_info)
