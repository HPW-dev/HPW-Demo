#include "menu.hpp"
#include "item/item.hpp"
#include "util/log.hpp"
#include "game/util/keybits.hpp"
#include "game/scene/scene-manager.hpp"
#include "graphic/font/font.hpp"

Menu::Menu(CN<Menu_items> items)
: m_items(items)
{ iflog(m_items.empty(), "m_items empty\n"); }

void Menu::update(double dt) {
  return_if(m_items.empty());

  if (is_pressed_once(hpw::keycode::enable))
    m_items[m_cur_item]->enable();
  if (is_pressed_once(hpw::keycode::left))
    m_items[m_cur_item]->minus();
  if (is_pressed_once(hpw::keycode::right))
    m_items[m_cur_item]->plus();
  if (is_pressed_once(hpw::keycode::down))
    next_item();
  if (is_pressed_once(hpw::keycode::up))
    prev_item();
} // update

void Menu::prev_item() {
  return_if(m_items.empty());
  iflog(m_items.empty(), "Menu.prev_item: m_items is empty");

  if (m_cur_item == 0) {
    m_cur_item = m_items.size() - 1;
    return;
  }
  --m_cur_item;
}

void Menu::next_item() {
  return_if(m_items.empty());
  iflog(m_items.empty(), "Menu.next_item: m_items is empty");
  
  ++m_cur_item;
  if (m_cur_item >= m_items.size())
    m_cur_item = 0;
}

std::size_t Menu::get_cur_item_id() const { return m_cur_item; }
CN<decltype(Menu::m_items)::value_type> Menu::get_cur_item() const { return m_items.at(m_cur_item); }
CN<decltype(Menu::m_items)> Menu::get_items() const { return m_items; }
