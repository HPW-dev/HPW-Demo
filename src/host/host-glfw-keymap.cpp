#ifdef WINDOWS
  #define GLFW_DLL
#endif
#include <GLFW/glfw3.h>

#include "host-glfw-keymap.hpp"
#include "windows-keycodes.hpp"
#include "game/util/keybits.hpp"
#include "util/str.hpp"

Key_mapper::Key_mapper() {
  reset();
} // c-tor

void Key_mapper::bind(hpw::keycode dst_hpw_key, int scancode) {
  nauto dst = table[dst_hpw_key];
  dst.scancode = scancode;
  utf32 name = n2s<utf32>(scancode);
  // получить читаемое название кнопки
  if (auto win_name = get_windows_keycode_name(scancode); win_name) {
    name = *win_name;
  } else { // если имя не найдено, то попытаться найти его в GLFW
    auto glfw_key_name = glfwGetKeyName(GLFW_KEY_UNKNOWN, dst.scancode);
    if (glfw_key_name)
      name = sconv<utf32>(Str(glfw_key_name)) + U" (" + n2s<utf32>(dst.scancode) + U")";
  }
  dst.name = name;
}

Keys_info Key_mapper::get_info() const {
  Keys_info info;
  for (cnauto [hpw_key, glfw_key]: table) {
    info.keys.emplace_back(Keys_info::Item {
      .hpw_key = hpw_key,
      .name = glfw_key.name,
      .scancode = glfw_key.scancode
    });
  }
  return info;
}

void Key_mapper::reset() { 
  // бинды по умолчанию
  bind(hpw::keycode::up,           glfwGetKeyScancode(GLFW_KEY_UP));
  bind(hpw::keycode::down,         glfwGetKeyScancode(GLFW_KEY_DOWN));
  bind(hpw::keycode::left,         glfwGetKeyScancode(GLFW_KEY_LEFT));
  bind(hpw::keycode::right,        glfwGetKeyScancode(GLFW_KEY_RIGHT));
  bind(hpw::keycode::focus,        glfwGetKeyScancode(GLFW_KEY_LEFT_ALT));
  bind(hpw::keycode::shoot,        glfwGetKeyScancode(GLFW_KEY_S));
  bind(hpw::keycode::bomb,         glfwGetKeyScancode(GLFW_KEY_X));
  bind(hpw::keycode::mode,         glfwGetKeyScancode(GLFW_KEY_A));
  bind(hpw::keycode::enable,       glfwGetKeyScancode(GLFW_KEY_ENTER));
  bind(hpw::keycode::escape,       glfwGetKeyScancode(GLFW_KEY_ESCAPE));

  bind(hpw::keycode::reset,        glfwGetKeyScancode(GLFW_KEY_F5));
  bind(hpw::keycode::fast_forward, glfwGetKeyScancode(GLFW_KEY_F4));
  bind(hpw::keycode::debug,        glfwGetKeyScancode(GLFW_KEY_F3));
  bind(hpw::keycode::console,      glfwGetKeyScancode(GLFW_KEY_GRAVE_ACCENT));
  bind(hpw::keycode::text_delete,  glfwGetKeyScancode(GLFW_KEY_BACKSPACE));
  bind(hpw::keycode::fps,          glfwGetKeyScancode(GLFW_KEY_F1));
  bind(hpw::keycode::fulscrn,      glfwGetKeyScancode(GLFW_KEY_F11));
  bind(hpw::keycode::screenshot,   glfwGetKeyScancode(GLFW_KEY_F2));
} // reset
