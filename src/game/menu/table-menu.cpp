#include <cassert>
#include <cmath>
#include "table-menu.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/util/graphic-util.hpp"
#include "graphic/font/font.hpp"
#include "item/table-row-item.hpp"
#include "game/core/fonts.hpp"

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

    cauto items = base->get_items();
    assert( !items.empty());
    // тест на то, что в таблице только табличные элементы нужного формата
    for (cnauto item: items) {
      cauto ptr = dcast< CP<Menu_item_table_row> >(item.get());
      assert(ptr);
    }
  } // Impl c-tor

  inline void draw(Image& dst) const {
    dst.fill(Pal8::black);
    graphic::font->draw(dst, Vec(8, 8), m_title);

    // TODO
  }

  inline void update(double dt) {}

}; // impl

Table_menu::Table_menu(CN<utf32> title, CN<Rows> rows, const uint row_height,
CN<Menu_items> items)
: Menu {items}
, impl {new_unique<Impl>(this, title, rows, row_height)}
{}

void Table_menu::draw(Image& dst) const { impl->draw(dst); }

void Table_menu::update(double dt) {
  Menu::update(dt);
  impl->update(dt);
}

Table_menu::~Table_menu() {}
