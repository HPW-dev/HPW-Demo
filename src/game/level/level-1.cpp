#include "level-1.hpp"
#include "game/game-font.hpp"
#include "graphic/image/image.hpp"
#include "graphic/font/font.hpp"

void Level_1::update(const Vec vel, double dt) {}

void Level_1::draw(Image& dst) const {
  graphic::font->draw(dst, {40, 70}, U"Заглушка уровень 1", &blend_past);
}
