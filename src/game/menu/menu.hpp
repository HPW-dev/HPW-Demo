#pragma once
#include "util/macro.hpp"
#include "util/vector-types.hpp"
#include "util/mem-types.hpp"

class Image;
class Menu_item;
using Menu_items = Vector<Shared<Menu_item>>;

/// база для реализаций менюшек
class Menu {
protected:
  Menu_items m_items {}; /// пункты меню
  std::size_t m_cur_item {}; /// текущий выбранный элемент

  void next_item(); /// перейти к следующему элементу
  void prev_item(); /// перейти к предыдущему элементу

public:
  Menu() = default;
  explicit Menu(CN<Menu_items> items);
  virtual ~Menu() = default;
  virtual void draw(Image& dst) const;
  virtual void update(double dt);
  std::size_t get_cur_item_id() const;
  CN<decltype(m_items)::value_type> get_cur_item() const;
  CN<decltype(m_items)> get_items() const;
};
