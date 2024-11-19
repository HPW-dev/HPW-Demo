#include <cassert>
#include "menu.hpp"
#include "item/item.hpp"
#include "util/log.hpp"
#include "game/core/common.hpp"
#include "game/util/keybits.hpp"
#include "game/scene/scene-mgr.hpp"
#include "graphic/font/font.hpp"

struct Sticking {
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

struct Menu::Impl {
  Menu& _master;
  Sticking _sticking {}; // действия при зажатии кнопок
  Menu_items _items {}; // пункты меню
  std::size_t _cur_item {}; // текущий выбранный элемент
  bool _item_selected {false};
  Menu_select_callback _select_callback {}; // вызывается всегда при выборе элемента меню
  Menu_select_callback _move_cursor_callback {}; // вызывается всегда при смене элемента в меню
  bool _key_holded {};

  inline explicit Impl(Menu& master, cr<Menu_items> items)
  : _master{master}
  , _items(items)
  {
    if (_items.empty())
      hpw_log("_items empty\n", Log_stream::warning);
  }

  // перейти к следующему элементу
  inline void next_item() {
    return_if(_items.empty());
    if (_items.empty())
      hpw_log("Menu.next_item: _items is empty", Log_stream::warning);
    
    ++_cur_item;
    if (_cur_item >= _items.size())
      _cur_item = 0;
  }

  // перейти к предыдущему элементу
  inline void prev_item() {
    return_if(_items.empty());
  if (_items.empty())
    hpw_log("Menu.prev_item: _items is empty\n", Log_stream::debug);

  if (_cur_item == 0) {
    _cur_item = _items.size() - 1;
    return;
  }
  --_cur_item;
  }

  inline bool holded() const { return _key_holded; }

  inline void update(const Delta_time dt) {
    return_if(_items.empty());

    for (crauto item: _items)
      item->update(dt);

    _sticking.update();
    _key_holded = false;

    if (_sticking.check(hpw::keycode::enable, 60, 60)) {
      _key_holded = true;
      _items[_cur_item]->enable();
      if (_select_callback)
        _select_callback(*_items[_cur_item]);
      _item_selected = true;
    } else {
      _item_selected = false;
    }
    
    if (_sticking.check(hpw::keycode::left, 50, 18)) {
      _key_holded = true;
      _items[_cur_item]->minus();
    }
    if (_sticking.check(hpw::keycode::right, 50, 18)) {
      _key_holded = true;
      _items[_cur_item]->plus();
    }

    if (_sticking.check(hpw::keycode::down, 60, 30)) {
      _key_holded = true;
      if (_move_cursor_callback)
        _move_cursor_callback(*_items[_cur_item]);
      next_item();
    }
    if (_sticking.check(hpw::keycode::up, 60, 30)) {
      _key_holded = true;
      if (_move_cursor_callback)
        _move_cursor_callback(*_items[_cur_item]);
      prev_item();
    }
  }

  inline void reset_sticking() { _sticking.reset(); }
  inline std::size_t get_cur_item_id() const { return _cur_item; }
  inline cr<Menu_items::value_type> get_cur_item() const { return _items.at(_cur_item); }
  inline cr<Menu_items> get_items() const { return _items; }
  inline bool item_selected() const { return _item_selected; }
  inline void set_select_callback(cr<Menu_select_callback> callback) { _select_callback = callback; }
  inline void set_move_cursor_callback(cr<Menu_select_callback> callback) { _move_cursor_callback = callback; }
}; // Impl

Menu::Menu(cr<Menu_items> items): _impl {new_unique<Impl>(*this, items)} {}
Menu::~Menu() {}
std::size_t Menu::get_cur_item_id() const { return _impl->get_cur_item_id(); }
cr<Menu_items::value_type> Menu::get_cur_item() const { return _impl->get_cur_item(); }
cr<Menu_items> Menu::get_items() const { return _impl->get_items(); }
void Menu::update(const Delta_time dt) { _impl->update(dt); }
bool Menu::item_selected() const { return _impl->item_selected(); }
void Menu::set_select_callback(cr<Menu_select_callback> callback) { _impl->set_select_callback(callback); }
void Menu::set_move_cursor_callback(cr<Menu_select_callback> callback) { _impl->set_move_cursor_callback(callback); }
void Menu::reset_sticking() { _impl->reset_sticking(); }
bool Menu::holded() const { return _impl->holded(); }
