#include "scene-info.hpp"
#include "game/core/fonts.hpp"
#include "game/util/keybits.hpp"
#include "game/core/scenes.hpp"
#include "game/scene/scene-manager.hpp"
#include "graphic/image/image.hpp"

Scene_info::Scene_info() {}

void Scene_info::update(const Delta_time dt) {
  if (is_pressed_once(hpw::keycode::escape))
    hpw::scene_mgr->back();
}

void Scene_info::draw(Image& dst) const {
  dst.fill(Pal8::black);
  graphic::font->draw(dst, {30, 30}, U"Заглушка окна с инфой", &blend_max);
}
