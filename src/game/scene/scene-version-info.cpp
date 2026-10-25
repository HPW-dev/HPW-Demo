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
    #ifdef WINDOWS
      #define sys_name U"Windows"
    #else
      #define sys_name U"Linux"
    #endif

    #ifdef is_x32
      #define sys_bitness U"x32"
    #else
      #define sys_bitness U"x64"
    #endif

    _msg += get_locale_str("common.author") + U" : "
      + utf8_to_32(get_game_build_author()) + U"\n";
    _msg += get_locale_str("version_info.game_ver") + U" : "
      + utf8_to_32(get_game_version()) + U"\n";
    _msg += get_locale_str("version_info.creation_date_time") + U" : "
      + utf8_to_32(get_game_creation_date()) + U" @ "
      + utf8_to_32(get_game_creation_time()) + U"\n";
    _msg += get_locale_str("version_info.for_system") + U" : "
      + sys_name + U" @ " + sys_bitness + U"\n";
    _msg += get_locale_str("version_info.build_preset") + U" : "
      + utf8_to_32(get_game_build_preset()) + U"\n";

    _msg += utf32(60, U'-') + U"\n";
    _msg += get_locale_str("common.exit") + U" : " + get_key_name(hpw::keycode::escape);
  }

  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape))
      hpw::scene_mgr.back();
  }

  inline void draw(Image& dst) const {
    dst.fill({});
    graphic::font->draw(dst, {25, 25}, _msg);
  }
}; // Impl

MAKE_SCENE_CLASS_IMPL(Scene_version_info)
