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

  /** Создаёт прокручиваемую интерактивную таблицу
  @param title название окна, можно пустое
  @param rows хедер таблицы
  @param row_height высота строки таблицы
  @param items контент таблицы
  @param elems_empty_txt табличка, которую покажут при отсутсвтии элементов списка */
  explicit Table_menu(cr<utf32> title, cr<Rows> rows, const uint row_height,
    cr<Menu_items> items, cr<utf32> elems_empty_txt={});
  ~Table_menu();
  void draw(Image& dst) const override;
};
