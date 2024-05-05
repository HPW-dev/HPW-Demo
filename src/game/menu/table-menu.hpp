#pragma once
#include "menu.hpp"
#include "util/unicode.hpp"
#include "util/str.hpp"
#include "util/math/num-types.hpp"

// Более красивое текстовое меню с описанием
class Table_menu: public Menu {
  struct Impl;
  Unique<Impl> impl {};

public:
  struct Row {
    utf32 name {}; // название столбца
    uint sz {}; // ширина столбца
  };
  using Rows = Vector<Row>;

  Table_menu() = default;
  ~Table_menu();
  explicit Table_menu(CN<utf32> title, CN<Rows> rows, const uint row_height,
    CN<Menu_items> items);
  void draw(Image& dst) const override;
};
