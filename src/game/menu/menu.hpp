#pragma once
#include <functional>
#include "util/macro.hpp"
#include "util/vector-types.hpp"
#include "util/mem-types.hpp"

class Image;
class Menu_item;
using Menu_items = Vector<Shared<Menu_item>>;
using Menu_select_callback = std::function<void (Menu_item&)>;

// база для реализаций менюшек
class Menu {
protected:
  Menu_items m_items {}; // пункты меню
  std::size_t m_cur_item {}; // текущий выбранный элемент
  bool m_item_selected {false};
  Menu_select_callback m_select_callback {}; // вызывается всегда при выборе элемента меню
  Menu_select_callback m_move_cursor_callback {}; // вызывается всегда при смене элемента в меню

  void next_item(); // перейти к следующему элементу
  void prev_item(); // перейти к предыдущему элементу

public:
  explicit Menu(CN<Menu_items> items);
  virtual ~Menu() = default;
  virtual void draw(Image& dst) const = 0;
  virtual void update(double dt);
  std::size_t get_cur_item_id() const;
  CN<decltype(m_items)::value_type> get_cur_item() const;
  CN<decltype(m_items)> get_items() const;
  bool item_selected() const; // проверить что был выбран один из пунктов меню
  void set_select_callback(CN<Menu_select_callback> callback);
  void set_move_cursor_callback(CN<Menu_select_callback> callback);
}; // Menu
