#include "entity-debug.hpp"
#include "entity.hpp"
#include "collidable.hpp"
#include "util/math/vec.hpp"
#include "game/core/fonts.hpp"
#include "game/core/debug.hpp"
#include "game/core/canvas.hpp"
#include "graphic/util/util-templ.hpp"

void Entity_debug::draw_pos(Image& dst, const Vec offset) const {
  auto pos = _master.phys.get_pos();
  utf32 pos_txt =
    U"(" +
    n2s<utf32>(pos.x, 1) +
    U", " +
    n2s<utf32>(pos.y, 1) +
    U")";
  graphic::font->draw(dst, pos + offset + Vec(5, 5), pos_txt, &blend_diff);
}

void Entity_debug::debug_draw(Image& dst, const Vec offset) const {
  if (graphic::draw_entity_pos) {
    // отрисовка позиций объекта
    draw_pos(dst, offset);
    // перекрестие по центру
    draw_line<&blend_diff>(dst, Vec(0, graphic::height/2.0),
      Vec(graphic::width, graphic::height/2.0), Pal8::white);
    draw_line<&blend_diff>(dst, Vec(graphic::width/2.0, 0),
      Vec(graphic::width/2.0, graphic::height), Pal8::white);
  }
  // показать жизни объекта
  if (graphic::draw_entity_hp && _master.status.collidable) {
    const Vec pos(_master.phys.get_pos() + Vec(15, 10));
    cauto casted = cptr2ptr<cp<Collidable>>(&_master);
    utf32 hp_text = U"HP: " + n2s<utf32>(casted->get_hp());
    graphic::font->draw(dst, pos, hp_text, &blend_diff);
  }
}
