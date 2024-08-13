#include <cassert>
#include "host-glfw-callback.hpp"
#include "host-glfw.hpp"
#include "host-glfw-keymap.hpp"
#include "game/core/common.hpp"
#include "game/core/graphic.hpp"
#include "game/core/user.hpp"
#include "util/error.hpp"

// вверх этот хедер не таскать, иначе всё развалится
#include "host-glfw-common.hpp"

void key_callback(GLFWwindow* /*m_window*/, int key, int scancode, int action, int mods) {
  hpw::any_key_pressed = true;
  assert(g_instance);
  cauto key_mapper = g_instance.load()->m_key_mapper.get();
  assert(key_mapper);
  nauto keymap_table = key_mapper->get_table();

  // режим ребинда клавиши
  if (g_rebind_key_mode) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
      key_mapper->bind(g_key_for_rebind, scancode);
      hpw::keys_info = key_mapper->get_info();
      g_rebind_key_mode = false;
      return; // чтобы нажатие не применилось в игровой логике
    }
  }

  // проверить нажатия на игровые клавиши
  for (cnauto [hpw_key, key]: keymap_table) {
    if (key.scancode == scancode) {
      if (action == GLFW_PRESS || action == GLFW_REPEAT)
        press(hpw_key);
      else // GLFW_RELEASE
        release(hpw_key);
    }
  }

  // альтернативная кнопка скриншота
  if (action == GLFW_PRESS && key == GLFW_KEY_PRINT_SCREEN)
    hpw::make_screenshot();
  // альтернативная кнопка фуллскрина
  if (action == GLFW_PRESS && key == GLFW_KEY_ENTER && mods == GLFW_MOD_ALT) {
    assert(hpw::set_fullscreen);
    hpw::set_fullscreen(!graphic::fullscreen);
  }
} // key_callback

// колбэк для ошибок нужен для GLFW
void error_callback(int error, Cstr description) {
  // поставить дефолтную гамму при ошибке
  if (g_instance && g_set_default_gamma_once) {
    assert(g_instance);
    g_instance.load()->set_gamma(1);
    g_set_default_gamma_once = false;
  }

  error("GLFW error: " << error << ": " << description);
}

void reshape_callback(GLFWwindow* /*m_window*/, int w, int h) {
  assert(g_instance);
  g_instance.load()->reshape(w, h);
}

// utf32 text input callback
void utf32_text_input_cb(GLFWwindow* /*m_window*/, std::uint32_t codepoint) {
  return_if (!hpw::text_input_mode);

  hpw::text_input_pos = std::clamp<int>(hpw::text_input_pos, 0, hpw::text_input.size());
  cauto ch = scast<decltype(hpw::text_input)::value_type>(codepoint);
  hpw::text_input.insert(hpw::text_input_pos, 1, ch);
  hpw::text_input_pos = std::min<int>(hpw::text_input_pos + 1, hpw::text_input.size());
}
