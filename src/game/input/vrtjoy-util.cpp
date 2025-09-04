#include <unordered_map>
#include "vrtjoy-util.hpp"
#include "util/macro.hpp"

Str key_name(Virtual_joystick::Key key) {
  static const std::unordered_map<Virtual_joystick::Key, Str> table {
    {Virtual_joystick::Key::A,      "A"},
    {Virtual_joystick::Key::B,      "B"},
    {Virtual_joystick::Key::ENABLE, "ENABLE"},
    {Virtual_joystick::Key::MENU,   "MENU"},
    {Virtual_joystick::Key::IGNORE, "{IGNORE}"},
  };
  
  cauto ret = table.find(key);
  ret_if (ret != table.end(), ret->second);
  return {};
}
