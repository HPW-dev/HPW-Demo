#include <cassert>
#include "menu.hpp"
#include "item/item.hpp"
#include "util/log.hpp"
#include "game/core/common.hpp"
#include "game/util/keybits.hpp"
#include "game/scene/scene-mgr.hpp"
#include "graphic/font/font.hpp"

struct Menu::Sticking {
  hpw::keycode last_pressed_key {hpw::keycode::error};
  bool use_sticking {}; // начать подавать нажатия импульсами
  uint hold_count {}; // сколько апдейтов зажимали клавишу

  /** true если кнопка нажата 1 раза или зажата.
  При зажатии результат будет импульсами
  @param keycode какую кнопку проверить
  @param updates_for_fast сколько апдейстов надо зажимать чтобы запустить быстрые нажатия
  @param fast_delay с какими интервалами учитывать зажатие клавиши */
  inline bool check(const hpw::keycode keycode,
  const uint updates_for_fast = 60, const uint fast_delay = 17) {
    assert(updates_for_fast > 0);
    assert(fast_delay > 0);
    return_if (keycode == hpw::keycode::error, false);

    // если ничё не жмём, выход
    if (!hpw::any_key_pressed) {
      reset();
      return false;
    }

    // когда на кнопку продолжают жать
    if (is_pressed_once(keycode)) {
      last_pressed_key = keycode;
      hold_count = 1;
      use_sticking = false;
      return true;
    } elif (is_pressed(keycode)) {
      // и кнопка та же самая
      if (last_pressed_key == keycode) {
        ++hold_count;
        // если зажали достаточное количество времени
        use_sticking = hold_count >= updates_for_fast;
        // генерировать нажатия импульсами
        if (use_sticking)
          return (hold_count % fast_delay) == 0;
      } else { // когда жмут на другую кнопку
        reset();
      }
      last_pressed_key = keycode;
    } // elif pressed keycode

    return false;
  } // check

  inline void update() {
    if (!is_any_key_pressed())
      reset();
  }

  inline void reset() {
    use_sticking = false;
    hold_count = 0;
    last_pressed_key = hpw::keycode::error;
  }
}; // Sticking

Menu::Menu(CN<Menu_items> items)
: m_sticking {new_unique<Sticking>()}
, m_items(items)
{ iflog(m_items.empty(), "m_items empty\n"); }

void Menu::update(const Delta_time dt) {
  return_if(m_items.empty());
  m_sticking->update();

  if (m_sticking->check(hpw::keycode::enable, 60, 60)) {
    m_items[m_cur_item]->enable();
    if (m_select_callback)
      m_select_callback(*m_items[m_cur_item]);
    m_item_selected = true;
  } else {
    m_item_selected = false;
  }
  
  if (m_sticking->check(hpw::keycode::left, 50, 18))
    m_items[m_cur_item]->minus();
  if (m_sticking->check(hpw::keycode::right, 50, 18))
    m_items[m_cur_item]->plus();

  if (m_sticking->check(hpw::keycode::down, 60, 30)) {
    if (m_move_cursor_callback)
      m_move_cursor_callback(*m_items[m_cur_item]);
    next_item();
  }
  if (m_sticking->check(hpw::keycode::up, 60, 30)) {
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
void Menu::reset_sticking() { m_sticking->reset(); }
