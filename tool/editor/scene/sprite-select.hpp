#pragma once
#include <functional>
#include <array>
#include "scene.hpp"
#include "util/mem-types.hpp"
#include "util/str.hpp"

class Sprite;

// меню выбора и просмотра ресурсов
class Sprite_select: public Editor_scene_base {
  // колбэк получит поинтер на выбранный спрайт и путь к нему
  std::function<void (Shared<Sprite>, CN<Str>)> callback {};
  Str sel_spr {};
  Strs _list {};
  std::array<char, 400> filter {};

  void enable();
  Strs get_list() const;
  int get_cur_list_idx(CN<Str> name, CN<Strs> list) const;

public:
  explicit Sprite_select(decltype(callback) new_callback={});
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
  void imgui_exec() override;
}; // Sprite_select
