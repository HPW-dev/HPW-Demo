#pragma once
#include <functional>
#include "util/macro.hpp"
#include "util/vector-types.hpp"
#include "util/mem-types.hpp"
#include "util/math/num-types.hpp"

class Image;
class Menu_item;
using Menu_items = Vector<Shared<Menu_item>>;
using Menu_select_callback = std::function<void (Menu_item&)>;

// база для реализаций менюшек
class Menu {
  struct Impl;
  Unique<Impl> _impl {};

public:
  explicit Menu(cr<Menu_items> items);
  virtual ~Menu();

  virtual void draw(Image& dst) const = 0;
  virtual void update(const Delta_time dt);
  std::size_t get_cur_item_id() const;
  cr<Menu_items::value_type> get_cur_item() const;
  cr<Menu_items> get_items() const;
  bool item_selected() const; // проверить что был выбран один из пунктов меню
  bool moved() const; // проверить что по пунктам меню было перемещение
  void set_select_callback(cr<Menu_select_callback> callback);
  void set_move_cursor_callback(cr<Menu_select_callback> callback);
  void reset_sticking(); // сброс залипания выбора
  void next_item(); // выбирает следующий пункт меню
  bool holded() const; // зажали кнопку в меню
};