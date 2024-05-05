#pragma once
#include "window.hpp"

// редактирование анимации
class Anim_wnd: public Window {
  void edit_start_deg();
  void make_new_frame();
  void delete_cur_frame();
  void move_cur_frame_up();
  void move_cur_frame_down();
  bool draw_frame_num(); // ret false if no frames in anim
  void draw_speed_scale();
  void draw_frame_dur();
  void copy_cur_frame_to_next();

public:
  Anim_wnd() = default;
  ~Anim_wnd() = default;
  inline void draw(Image& dst) const override {}
  void update(double dt) override;
  void imgui_exec() override;
};
