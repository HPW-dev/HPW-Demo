#include "pch.hpp"
#include "glfw-input.hpp"

#include "glfw-context.hpp"
#include "glfw-include.hpp"
#include "port/windows/windows-keycodes.hpp"
#include "game/core/common.hpp"
#include "game/core/user.hpp"


void Key_mapper::bind(hpw::keycode dst_hpw_key, int scancode) {
  rauto dst = table[dst_hpw_key];
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
  for (crauto [hpw_key, glfw_key]: table) {
    info.keys.emplace_back(Keys_info::Item {
      .hpw_key = hpw_key,
      .name = glfw_key.name,
      .scancode = glfw_key.scancode
    });
  }
  return info;
}

void Key_mapper::reset() { 
  table.clear();

  // бинды по умолчанию
  bind(hpw::keycode::up,           glfwGetKeyScancode(GLFW_KEY_UP));
  bind(hpw::keycode::down,         glfwGetKeyScancode(GLFW_KEY_DOWN));
  bind(hpw::keycode::left,         glfwGetKeyScancode(GLFW_KEY_LEFT));
  bind(hpw::keycode::right,        glfwGetKeyScancode(GLFW_KEY_RIGHT));
  bind(hpw::keycode::focus,        glfwGetKeyScancode(GLFW_KEY_LEFT_ALT));
  bind(hpw::keycode::shoot,        glfwGetKeyScancode(GLFW_KEY_Z));
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
}


Lazy_press::Lazy_press(const hpw::keycode keycode)
: _keycode {keycode}
{}

void Lazy_press::update(const Delta_time dt) { 
  if (!_kill_me_on_next_update) {
    press(_keycode);
    _kill_me_on_next_update = true;
  } else {
    this->kill();
  }
}

void Lazy_press::on_end() { release(_keycode); }


void hotkey_process(GLFWwindow* window,
int key, int scancode, int action, int mods) {
  // альтернативная кнопка скриншота
  if (action == GLFW_PRESS && key == GLFW_KEY_PRINT_SCREEN)
    hpw::task_mgr.add(new_shared<Lazy_press>(hpw::keycode::screenshot));
  
  // альтернативная кнопка фуллскрина
  if (action == GLFW_PRESS && key == GLFW_KEY_ENTER && mods == GLFW_MOD_ALT) {
    hpw::task_mgr.add(new_shared<Lazy_press>(hpw::keycode::fulscrn));
    release(hpw::keycode::enable);
  }
  
  // вставка текста из буффера Ctrl + V
  if (action == GLFW_PRESS && key == GLFW_KEY_V && mods == GLFW_MOD_CONTROL) {
    if (hpw::text_input_mode) {
      cauto str_for_buf = glfwGetClipboardString(window);
      if (str_for_buf) {
        const Str buffer(str_for_buf);
        hpw::text_input.insert(hpw::text_input_pos, utf8_to_32(buffer));
        hpw::text_input_pos = std::min<int>(hpw::text_input_pos + buffer.size(),
          hpw::text_input.size());
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

void mouse_button_cb(GLFWwindow* window,
int button, int action, int mods) {
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

void key_cb(GLFWwindow* window, int key, int scancode,
int action, int mods) {
  hpw::any_key_pressed = true;
  rauto keymap_table = g_ctx.key_mapper.get_table();

  // режим ребинда клавиши
  if (g_ctx.rebind_key_mode) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
      g_ctx.key_mapper.bind(g_ctx.key_for_rebind, scancode);
      hpw::keys_info = g_ctx.key_mapper.get_info();
      g_ctx.rebind_key_mode = false;
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

void utf32_text_input_cb(GLFWwindow* __blcic_u32, std::uint32_t codepoint) {
  return_if (!hpw::text_input_mode);

  hpw::text_input_pos = std::clamp<int>(hpw::text_input_pos, 0, hpw::text_input.size());
  cauto ch = scast<decltype(hpw::text_input)::value_type>(codepoint);
  hpw::text_input.insert(hpw::text_input_pos, 1, ch);
  hpw::text_input_pos = std::min<int>(hpw::text_input_pos + 1, hpw::text_input.size());
}

void rebind_key(hpw::keycode hpw_key) {
  g_ctx.rebind_key_mode = true;
  g_ctx.key_for_rebind = hpw_key;
};

void rebind_key_by_scancode(hpw::keycode hpw_key, int scancode) {
  g_ctx.key_mapper.bind(hpw_key, scancode);
  hpw::keys_info = g_ctx.key_mapper.get_info();
};
  
void reset_keymap() {
  log_debug << "reseting keymap...";
  g_ctx.key_mapper.reset();
  hpw::keys_info = g_ctx.key_mapper.get_info();
};

