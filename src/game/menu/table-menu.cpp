#include <cassert>
#include <cmath>
#include "table-menu.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/util/graphic-util.hpp"
#include "graphic/font/font.hpp"
#include "item/table-row-item.hpp"
#include "game/core/fonts.hpp"
#include "game/util/game-util.hpp"

struct Table_menu::Impl {
  Menu* m_base {};
  utf32 m_title {}; /// название всего меню
  Rows m_rows {};
  uint m_row_height {};

  inline explicit Impl(Menu* base, CN<utf32> title, CN<Rows> rows,
  const uint row_height)
  : m_base {base}
  , m_title {title}
  , m_rows {rows}
  , m_row_height {row_height}
  {
    assert(m_base);
    assert(m_row_height > 0);
    assert( !m_title.empty());
    assert( !m_rows.empty());

    // тест на то, что в таблице только табличные элементы нужного формата
    cauto items = base->get_items();
    for (cnauto item: items) {
      cauto ptr = dcast< CP<Menu_item_table_row> >(item.get());
      assert(ptr);
    }
  } // Impl c-tor

  inline void draw(Image& dst) const {
    dst.fill(Pal8::black);
    graphic::font->draw(dst, Vec(8, 8), m_title);
    cauto table_offset_y = 28u;
    draw_table(dst, table_offset_y);
  }

  inline void draw_table(Image& dst, const uint table_offset_y) const {
    Vec pos(0, table_offset_y);
    constexpr auto text_offset = Vec(7, 7);

    // показать заголовок таблицы
    for (cnauto row: m_rows) {
      // если размер 0, то значит размер элемента до конца строки
      auto row_sz = (row.sz > 0) ? row.sz : (dst.X - pos.x);
      const Rect rect(pos, Vec(row_sz, m_row_height));
      draw_rect(dst, rect, Pal8::gray);
      graphic::font->draw(dst, pos + text_offset, row.name, &blend_158);
      pos.x += row_sz - 1;
    }

    pos.y += m_row_height - 1; // отступ от хедера

    cauto items = m_base->get_items();
    if (items.empty()) {
      graphic::font->draw(dst, pos + text_offset, get_locale_str("scene.replay.no_replay"));
      return;
    }

    // после нескольких строк двигать список
    const uint item_idx_start = std::max<int>(0, m_base->get_cur_item_id() - 6);
    // нарисовать элементы таблицы
    for (auto item_idx = item_idx_start; item_idx < items.size(); ++item_idx) {
      cnauto item = items[item_idx];
      break_if (pos.y > dst.Y); // не надо показывать таблицу за пределами экрана
      cauto selected = item == this->m_base->get_cur_item();

      pos.x = 0;
      // геттеры получают контент в строках таблицы
      cauto item_row = dcast<CP<Menu_item_table_row>>(item.get());
      assert(item_row);
      cnauto content_getters = item_row->get_content_getters();
      
      // нарисовать столько столбцов, сколько было задано при ините
      for (uint i = 0; cnauto row: m_rows) {
        // если размер 0, то значит размер элемента до конца строки
        auto row_sz = (row.sz > 0) ? row.sz : (dst.X - pos.x);
        const Rect rect(pos, Vec(row_sz, m_row_height));
        cauto rect_color = selected
          ? Pal8::white
          : Pal8::gray;
        draw_rect<&blend_max>(dst, rect, rect_color);

        // не рисовать контент, если его нет
        if (i < content_getters.size()) {
          cauto getter = content_getters[i];
          if (getter) {
            cauto text_color = selected
              ? &blend_past
              : &blend_158;
            graphic::font->draw(dst, pos + text_offset, getter(), text_color);
          }
        }
        pos.x += row_sz - 1;
        ++i;
      } // for rows
      pos.y += m_row_height - 1; // след. строка
    } // for items
  } // draw_table
}; // impl

Table_menu::Table_menu(CN<utf32> title, CN<Rows> rows, const uint row_height,
CN<Menu_items> items)
: Menu {items}
, impl {new_unique<Impl>(this, title, rows, row_height)}
{}

void Table_menu::draw(Image& dst) const { impl->draw(dst); }
Table_menu::~Table_menu() {}
