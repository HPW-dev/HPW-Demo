#include <cassert>
#include <cmath>
#include <algorithm>
#include "table-menu.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/util/graphic-util.hpp"
#include "item/table-row-item.hpp"
#include "game/core/fonts.hpp"

struct Table_menu::Impl {
  Menu* m_base {};
  utf32 m_title {}; // название всего меню
  Rows m_rows {};
  uint m_row_height {};
  utf32 m_elems_empty_txt {}; // табличка, которую покажут при отсутсвтии элементов списка

  inline explicit Impl(Menu* base, cr<utf32> title, cr<Rows> rows,
  const uint row_height, cr<utf32> elems_empty_txt)
  : m_base {base}
  , m_title {title}
  , m_rows {rows}
  , m_row_height {row_height}
  , m_elems_empty_txt {elems_empty_txt}
  {
    assert(m_base);
    assert(m_row_height > 0);
    assert( !m_title.empty());
    assert( !m_rows.empty());

    // тест на то, что в таблице только табличные элементы нужного формата
    #ifdef DEBUG
      cauto items = base->get_items();
      for (crauto item: items) {
        cauto ptr = dcast< cp<Menu_item_table_row> >(item.get());
        assert(ptr);
      }
    #endif
  } // Impl c-tor

  inline void draw(Image& dst) const {
    dst.fill(Pal8::black);
    assert(graphic::font);
    graphic::font->draw(dst, Vec(8, 8), m_title);
    cauto table_offset_y = 28u;
    draw_table(dst, table_offset_y);
  }

  inline void draw_table(Image& dst, const uint table_offset_y) const {
    Vec pos(0, table_offset_y);
    constexpr auto text_offset = Vec(7, 7);

    // показать заголовок таблицы
    for (crauto row: m_rows) {
      // если размер 0, то значит размер элемента до конца строки
      auto row_sz = (row.sz > 0) ? row.sz : (dst.X - pos.x);
      const Rect rect(pos, Vec(row_sz, m_row_height));
      draw_rect(dst, rect, Pal8::gray);
      graphic::font->draw(dst, pos + text_offset, row.name, &blend_158);
      pos.x += row_sz - 1;
    }

    pos.y += m_row_height - 1; // отступ от хедера

    // если элементов нет, то надпись
    cauto items = m_base->get_items();
    if (items.empty() && !m_elems_empty_txt.empty()) {
      pos.x = 0;
      graphic::font->draw(dst, pos + text_offset, m_elems_empty_txt);
      return;
    }

    // после нескольких строк двигать список
    const uint item_idx_start = std::max<int>(0, m_base->get_cur_item_id() - 6);
    // нарисовать элементы таблицы
    for (auto item_idx = item_idx_start; item_idx < items.size(); ++item_idx) {
      crauto item = items[item_idx];
      break_if (pos.y > dst.Y); // не надо показывать таблицу за пределами экрана
      cauto selected = item == this->m_base->get_cur_item();

      pos.x = 0;
      // геттеры получают контент в строках таблицы
      cauto item_row = dcast<cp<Menu_item_table_row>>(item.get());
      assert(item_row);
      crauto content_getters = item_row->get_content_getters();
      
      // нарисовать столько столбцов, сколько было задано при ините
      for (uint i = 0; crauto row: m_rows) {
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

Table_menu::Table_menu(cr<utf32> title, cr<Rows> rows, const uint row_height,
cr<Menu_items> items, cr<utf32> elems_empty_txt)
: Menu {items}
, impl {new_unique<Impl>(this, title, rows, row_height, elems_empty_txt)}
{}

void Table_menu::draw(Image& dst) const { impl->draw(dst); }
Table_menu::~Table_menu() {}
