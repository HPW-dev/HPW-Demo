#pragma once
#include <functional>
#include <array>
#include "scene.hpp"
#include "util/str-util.hpp"

/// окно ввода названий
class Input_scene: public Editor_scene_base {
  nocopy(Input_scene);
  Str title {};
  /// вызываемая функция через строку получит введёные символы
  using callback_t = std::function<void (CN<Str>)>;
  callback_t callback {};
  std::array<char, 256> charbuf {};

public:
  Input_scene(CN<Str> new_title, CN<callback_t> new_callback);
  void update(double dt) override;
  void imgui_exec() override;
}; // Input_scene
