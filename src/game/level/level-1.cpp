#include "level-1.hpp"
#include "game/core/fonts.hpp"
#include "graphic/image/image.hpp"

void Level_1::update(const Vec vel, Delta_time dt) {}

void Level_1::draw(Image& dst) const {
  graphic::font->draw(dst, {40, 70}, U"Заглушка уровень 1", &blend_past);
}
