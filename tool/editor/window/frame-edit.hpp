#pragma once
#include "window.hpp"
#include "util/mempool.hpp"

class Frame;
class Hitbox;
struct Polygon;

// редактирование отдельного кадра анимации
class Frame_wnd: public Window {
  bool reinit {false};

  void draw_info(CN<Frame> frame);
  void draw_path_edit(Frame& frame);
  void draw_direction_count_edit(Frame& frame);
  void draw_offset_edit(Frame& frame);
  void draw_downscale_tune_edit(Frame& frame);
  void draw_duration_edit(Frame& frame);

  void edit_hitbox(Frame& frame);
  // хелпер-функция для получения редактируемого хитбокса
  Pool_ptr(Hitbox) get_hitbox_source() const;
  void update_hitbox(Pool_ptr(Hitbox) dst);
  // true, если в хитбкосе надо обновить изменения
  bool edit_polygon(Pool_ptr(Hitbox) hitbox) const;
  // true, если в хитбкосе надо обновить изменения
  bool edit_polygon_detailed(Pool_ptr(Hitbox) hitbox) const;
  // true, если в хитбкосе надо обновить изменения
  bool edit_polygon_offset(Polygon& poly) const;
  // true, если в хитбкосе надо обновить изменения
  bool edit_inserter_polygon_points(Polygon& poly) const;

public:
  Frame_wnd() = default;
  ~Frame_wnd() = default;
  inline void draw(Image& dst) const override {}
  inline void update(double dt) override {}
  void imgui_exec() override;
}; // Frame_wnd
