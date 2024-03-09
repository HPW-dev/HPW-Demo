#pragma once
#include "menu.hpp"
#include "util/unicode.hpp"
#include "util/str.hpp"

/// Более красивое текстовое меню с описанием
class Table_menu: public Menu {
  struct Impl;
  Unique<Impl> impl {};

public:
  Table_menu() = default;
  ~Table_menu();
  explicit Table_menu(CN<utf32> title, CN<Strs> row_names,
    CN<Menu_items> rows);
  void draw(Image& dst) const override;
  void update(double dt) override;
};
