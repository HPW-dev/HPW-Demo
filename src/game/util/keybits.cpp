#include "keybits.hpp"

void press(hpw::keycode code) {
  hpw::keys.at(scast<std::size_t>(code)).cur = true;
}

void release(hpw::keycode code) {
  hpw::keys.at(scast<std::size_t>(code)).cur = false;
}

void clear_cur_keys() {
  for (rauto key: hpw::keys)
    key.cur = false;
}

void keys_cur_to_prev() {
  for (rauto key: hpw::keys)
    key.prev = key.cur;
}

bool is_pressed(hpw::keycode code) {
  return hpw::keys.at(scast<std::size_t>(code)).cur;
}

bool is_pressed_once(hpw::keycode code) {
  crauto key = hpw::keys.at(scast<std::size_t>(code));
  return key.cur == true && key.prev == false;
}

cp<Keys_info::Item> Keys_info::find(hpw::keycode hpw_key) const {
  crauto item = std::find_if(keys.begin(), keys.end(), [hpw_key](auto it)->bool
    { return it.hpw_key == hpw_key; });
  if (item != keys.end())
    return &(*item);
  return nullptr;
}

bool is_any_key_pressed() {
  for (crauto key: hpw::keys) {
    if (key.cur)
      return true;
  }
  return false;
}
