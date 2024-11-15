#include <cassert>
#include "host-glfw-callback.hpp"
#include "host-glfw.hpp"
#include "host-glfw-keymap.hpp"
#include "game/core/common.hpp"
#include "game/core/graphic.hpp"
#include "game/core/user.hpp"
#include "game/core/tasks.hpp"
#include "util/error.hpp"

// отложенное нажатие кнопки
class Lazy_press final: public Task {
  hpw::keycode _keycode {};
  bool _kill_me_on_next_update {};

public:
  inline explicit Lazy_press(const hpw::keycode keycode): _keycode {keycode} {}

  inline void update(const Delta_time dt) override final { 
    if (!_kill_me_on_next_update) {
      press(_keycode);
      _kill_me_on_next_update = true;
    } else
      this->kill();
  }

  inline void on_end() {
    release(_keycode);
  }
}; // Lazy_press

// вверх этот хедер не таскать, иначе всё развалится
#include "host-glfw-common.hpp"

static void hotkey_process(GLFWwindow* window, int key, int scancode, int action, int mods) {
  // альтернативная кнопка скриншота
  if (action == GLFW_PRESS && key == GLFW_KEY_PRINT_SCREEN)
    hpw::global_task_mgr.add(new_shared<Lazy_press>(hpw::keycode::screenshot));
  
  // альтернативная кнопка фуллскрина
  if (action == GLFW_PRESS && key == GLFW_KEY_ENTER && mods == GLFW_MOD_ALT) {
    hpw::global_task_mgr.add(new_shared<Lazy_press>(hpw::keycode::fulscrn));
    release(hpw::keycode::enable);
  }
  
  // вставка текста из буффера Ctrl + V
  if (action == GLFW_PRESS && key == GLFW_KEY_V && mods == GLFW_MOD_CONTROL) {
    if (hpw::text_input_mode) {
      const Str buffer = glfwGetClipboardString(window);
      if (!buffer.empty()) {
        hpw::text_input.insert(hpw::text_input_pos, utf8_to_32(buffer));
        hpw::text_input_pos = std::min<int>(hpw::text_input_pos + buffer.size(), hpw::text_input.size());
      }
    }
  }

  // снос всей введёной строки текста (Ctrl + C)
  if (action == GLFW_PRESS && key == GLFW_KEY_C && mods == GLFW_MOD_CONTROL) {
    if (hpw::text_input_mode) {
      hpw::text_input.clear();
      hpw::text_input_pos = 0;
    }
  }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
  // вставка текста из буффера при правой кнопке мыши
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
    if (hpw::text_input_mode) {
      const Str buffer = glfwGetClipboardString(window);
      if (!buffer.empty()) {
        hpw::text_input.insert(hpw::text_input_pos, utf8_to_32(buffer));
        hpw::text_input_pos = std::min<int>(hpw::text_input_pos + buffer.size(), hpw::text_input.size());
      }
    }
  }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  hpw::any_key_pressed = true;
  assert(g_instance);
  cauto key_mapper = g_instance.load()->m_key_mapper.get();
  assert(key_mapper);
  rauto keymap_table = key_mapper->get_table();

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
  for (crauto [hpw_key, key]: keymap_table) {
    if (key.scancode == scancode) {
      if (action == GLFW_PRESS || action == GLFW_REPEAT)
        press(hpw_key);
      else // GLFW_RELEASE
        release(hpw_key);
    }
  }

  hotkey_process(window, key, scancode, action, mods);
}

// колбэк для ошибок нужен для GLFW
void error_callback(int error, Cstr description) {
  // поставить дефолтную гамму при ошибке
  if (g_instance && g_set_default_gamma_once) {
    assert(g_instance);
    g_instance.load()->set_gamma(1);
    g_set_default_gamma_once = false;
  }

  Str addition_desc;
  if (error == 65540)
    addition_desc += ". Maybe need delete config.yml";

  error("GLFW error: " << error << ": " << description << addition_desc);
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
