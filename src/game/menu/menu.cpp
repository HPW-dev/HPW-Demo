#include "menu.hpp"
#include "item/item.hpp"
#include "util/log.hpp"
#include "game/util/keybits.hpp"
#include "game/scene/scene-manager.hpp"
#include "graphic/font/font.hpp"

struct Menu::Sticking {

};

// сколько апдейтов надо подождать, чтоы зажатие сработало
static uint updates_threshold = 60;
static auto last_pressed_keycode = hpw::keycode::error;
static uint hold_count = 0;

// учитывает одиночное нажатие или зажатие клавиши
bool check_pressed_or_holded (
const hpw::keycode keycode,
const uint UPDATES_THRESHOLD = 60,
const uint FAST_UPDATES_THRESHOLD = 17
) {
  updates_threshold = UPDATES_THRESHOLD;
  bool ret = false;

  if (is_pressed_once(keycode)) {
    ret = true;
    last_pressed_keycode = keycode;
    hold_count = 0;
    // вернуть обратно число тиков, для применения автонажатия
    updates_threshold = UPDATES_THRESHOLD;
  } else {
    if (is_pressed(keycode)) {
      if (last_pressed_keycode == keycode)
        ++hold_count;
      else {
        hold_count = 0;
        // вернуть обратно число тиков, для применения автонажатия
        updates_threshold = UPDATES_THRESHOLD;
      }
      last_pressed_keycode = keycode;
    }
  }
  if (hold_count >= updates_threshold) {
    hold_count = 0;
    ret = true;
    // ускорить автонажатие
    updates_threshold = FAST_UPDATES_THRESHOLD;
  }
  return ret;
} // check_pressed_or_holded

Menu::Menu(CN<Menu_items> items)
: m_sticking {new_unique<Sticking>()}
, m_items(items)
{ 
  iflog(m_items.empty(), "m_items empty\n");
  // сброс зажатых кнопок
  last_pressed_keycode = hpw::keycode::error;
  hold_count = 0;
}

void Menu::update(double dt) {
  return_if(m_items.empty());

  if (check_pressed_or_holded(hpw::keycode::enable, 60, 60)) {
    m_items[m_cur_item]->enable();
    if (m_select_callback)
      m_select_callback(*m_items[m_cur_item]);
    m_item_selected = true;
  } else {
    m_item_selected = false;
  }
  
  if (check_pressed_or_holded(hpw::keycode::left, 50, 18))
    m_items[m_cur_item]->minus();
  if (check_pressed_or_holded(hpw::keycode::right, 50, 18))
    m_items[m_cur_item]->plus();

  if (check_pressed_or_holded(hpw::keycode::down, 60, 30)) {
    if (m_move_cursor_callback)
      m_move_cursor_callback(*m_items[m_cur_item]);
    next_item();
  }
  if (check_pressed_or_holded(hpw::keycode::up, 60, 30)) {
    if (m_move_cursor_callback)
      m_move_cursor_callback(*m_items[m_cur_item]);
    prev_item();
  }
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

Menu::~Menu() {}
std::size_t Menu::get_cur_item_id() const { return m_cur_item; }
CN<decltype(Menu::m_items)::value_type> Menu::get_cur_item() const { return m_items.at(m_cur_item); }
CN<decltype(Menu::m_items)> Menu::get_items() const { return m_items; }
bool Menu::item_selected() const { return m_item_selected; }
void Menu::set_select_callback(CN<Menu_select_callback> callback) { m_select_callback = callback; }
void Menu::set_move_cursor_callback(CN<Menu_select_callback> callback) { m_move_cursor_callback = callback; }
