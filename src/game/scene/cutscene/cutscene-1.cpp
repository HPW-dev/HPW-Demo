#include "cutscene-1.hpp"
#include "game/util/keybits.hpp"
#include "game/core/scenes.hpp"
#include "graphic/image/image.hpp"
#include "game/scene/scene-mgr.hpp"
#include "game/core/fonts.hpp"

struct Cutscene_1::Impl {


  inline Impl() {

  }

  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape))
      hpw::scene_mgr->back();
  }

  inline void draw(Image& dst) const {  
    dst.fill(Pal8::black);
    graphic::font->draw(dst, {20, 30}, U"Заглушка катсцены 1", &blend_max);
  }
};

Cutscene_1::Cutscene_1(): impl{new_unique<Impl>()} {}
Cutscene_1::~Cutscene_1() {}
void Cutscene_1::update(const Delta_time dt) { impl->update(dt); }
void Cutscene_1::draw(Image& dst) const { impl->draw(dst); }
