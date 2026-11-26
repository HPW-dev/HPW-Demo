#pragma once
#include <unordered_map>

#include "game/core/tasks.hpp"
#include "game/input/keys.hpp"
#include "util/macro.hpp"
#include "util/str.hpp"


class Key_mapper final {
  nocopy(Key_mapper);

  struct GLFW_key {
    utf32 name {};
    int scancode {}; // системный код клавиши
  };

  // <hpw key, GLFW key>
  std::unordered_map<hpw::keycode, GLFW_key> table {};

public:
  Key_mapper() = default;
  ~Key_mapper() = default;
  void reset(); // поставить клавиши по умолчанию
  void bind(hpw::keycode dst_hpw_key, int glfw_key); // забиндить клавишу
  inline decltype(table)& get_table() { return table; }
  Keys_info get_info() const;
};

// отложенное нажатие кнопки
class Lazy_press final: public Task {
  hpw::keycode _keycode {};
  bool _kill_me_on_next_update {};

public:
  explicit Lazy_press(const hpw::keycode keycode);
  void update(const Delta_time dt) override final;
  void on_end();
};


struct GLFWwindow;

void hotkey_process(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_cb(GLFWwindow* window, int button, int action, int mods);
void key_cb(GLFWwindow* window, int key, int scancode, int action, int mods);
void utf32_text_input_cb(GLFWwindow* _, std::uint32_t codepoint);
