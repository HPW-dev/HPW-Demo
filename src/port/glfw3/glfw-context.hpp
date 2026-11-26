#pragma once
#include <optional>
#include <atomic>

#include "game/core/canvas.hpp"
#include "glfw-input.hpp"


struct GLFWwindow;

// Все нужные данные для GLFW окна
struct Glfw3_context {
  int argc {};
  char** argv {};

// GLFW окно:
  GLFWwindow* window {};
  Veci wnd_off {440, 240}; // Стартовое смещение окна
  Veci wnd_sz {graphic::width, graphic::height}; // реальная ширина и высота окна (меняется при ресайзе)
  Recti wnd_game_area {}; // видимая часть внутреннего окна игры
  Str window_title {"H.P.W"};
  std::optional<bool> start_fullscreen {}; // через аргументы запуска можно указать запуститься в фулскрине
  Key_mapper key_mapper {};

// OpenGL:
  Vector<float> pal_rgb {}; // Палитра игры подготовленная к отправке на видяху
  uint screen_tex = 0; // OpenGL текстура экрана
  uint vao = 0;
  uint pbos[2] = {0, 0};
  bool use_double_pbo {true};
  int pbo_index = 0;
  size_t pbo_sz = 0;
  uint shader_prog = 0, vert_shader = 0, frag_shader = 0;
  uint pal_tex = 0;
  cp<void> pixels {}; // данные для копирования в текстуру

// Общее для игры:
  std::atomic_bool is_ran {true};
  bool rebind_key_mode {false};
  hpw::keycode key_for_rebind {hpw::keycode::error}; // появится при hpw::rebind_key
  bool once_gamma_conf {true}; // защита от циклического выставления гаммы
  std::optional<uint32_t> custom_seed {};
};

inline Glfw3_context g_ctx {};
