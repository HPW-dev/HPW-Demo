#include "scene-info.hpp"
#include "game/game-font.hpp"
#include "game/util/keybits.hpp"
#include "game/util/game-scenes.hpp"
#include "game/scene/scene-manager.hpp"
#include "graphic/font/font.hpp"
#include "graphic/image/image.hpp"

Scene_info::Scene_info() {}

void Scene_info::update(double dt) {
  if (is_pressed_once(hpw::keycode::escape))
    hpw::scene_mgr->back();
}

void Scene_info::draw(Image& dst) const {
  dst.fill(Pal8::black);
  graphic::font->draw(dst, {30, 30}, U"Заглушка окна с инфой", &blend_max);
}
