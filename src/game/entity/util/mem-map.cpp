#include "mem-map.hpp"
#include "util/math/vec.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/graphic-util.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/font/font.hpp"
#include "game/entity/entity-manager.hpp"
#include "game/entity/entity.hpp"
#include "game/entity/collidable.hpp"
#include "game/entity/player.hpp"
#include "game/entity/particle.hpp"
#include "game/game-common.hpp"
#include "game/game-font.hpp"

void draw_entity_mem_map(Image& dst, const Vec pos) {

  // показать окошко
  cauto window = Rect(pos, Vec(300, 200));
  draw_rect<&blend_diff>(dst, window, Pal8::white);
  draw_rect_filled<&blend_158>(dst, window, Pal8::red_black);
  draw_rect_filled<&blend_158>(dst, window, Pal8::red_black);

  // цвета объектов в патблице
  constexpr Pal8 unknownd_entity_color = Pal8::white;
  constexpr Pal8 bullet_color = Pal8::red;
  constexpr Pal8 dead_color = Pal8::black;
  constexpr Pal8 particle_color = Pal8::gray;

  const Vec table_offset(10, 15);
  const Vec table_space(1, 1); // отступ между точками таблицы
  std::size_t pos_x {};
  std::size_t pos_y {};
  // размеры таблицы
  constexpr std::size_t table_mx = 140;
  constexpr std::size_t table_my = 80;
  // узнать сколько сейчас объектов живо и отобразить их в таблице
  cnauto entities = hpw::entity_mgr->get_entitys();
  std::size_t lived {};
  for (cnauto entity: entities) {
    Pal8 color = dead_color;

    if (entity->status.live) {
      ++lived;

      // определить цвет
      if (entity->type == ENTITY_TYPE(Collidable))
        color = bullet_color;
      else if (entity->type == ENTITY_TYPE(Particle))
        color = particle_color;
      else 
        color = unknownd_entity_color;
    } // if live

    if (pos_y < table_my) {
      // поставить точку в таблице
      dst.set (
        pos.x + table_offset.x + pos_x * (1 + table_space.x),
        pos.y + table_offset.y + pos_y * (1 + table_space.y),
        color
      );

      ++pos_x;
      if (pos_x >= table_mx) {
        pos_x = 0;
        ++pos_y;
      }
    }
  } // for entities

  // текстовая инфа
  utf32 txt;
  cauto allocated = hpw::entity_mgr->get_entity_pool().allocated();
  txt += U"Allocated: " + n2s<utf32>(allocated) + U" Byte ("
   + n2s<utf32>(scast<double>(allocated) / (1024 * 1024), 2) + U" Mb)\n";
   txt += U"Lived: " + n2s<utf32>(lived) + U" / " + n2s<utf32>(entities.size()) + U"\n";

  const Vec txt_offset(10, window.size.y - 42);
  graphic::font->draw(dst, pos + txt_offset, txt);

} // draw_entity_mem_map
