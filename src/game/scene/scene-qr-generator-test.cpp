#include "pch.hpp"
#include "scene-qr-generator-test.hpp"
#include "game/core/scenes.hpp"
#include "game/core/canvas.hpp"
#include "game/core/fonts.hpp"
#include "game/util/keybits.hpp"
#include "engine/graphic/util/qr-gen.hpp"
#include "engine/graphic/util/util-templ.hpp"

struct Scene_qr_generator_test::Impl {
  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape))
      hpw::scene_mgr.back();
  }

  inline void draw(Image& dst) const {
    dst.fill({});
    
    // надеюсь что сервера яндекса взорвут не полностью и этот сайт будет работать
    scauto URL = U"https://hpw-game.website.yandexcloud.net/";

    cauto POS = Veci(120, 45);
    assert(graphic::font);
    graphic::font->draw(dst, POS, URL);

    insert(dst, qr_gen(utf32_to_8(URL)), POS + Veci{0, 20});
  }
}; // Impl

MAKE_SCENE_CLASS_IMPL(Scene_qr_generator_test)
