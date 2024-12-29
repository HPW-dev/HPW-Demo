#pragma once
#include "window.hpp"
#include "util/macro.hpp"
#include "util/str.hpp"

class Heat_distort;

Strs spawner_type_names();

// настройки спавнера частиц
class Emit_wnd: public Window {
  void draw_anim_list();
  void make_new_anim();
  void delete_anim_by_name(cr<Str> name);
  void select_anim_by_name(cr<Str> name);
  void draw_entity_flags();
  void heat_distort_edit();
  void heat_distort_flags_edit(Heat_distort& heat_distort);
  void light_edit();
  void reset();
  void save_to_clipboard(cr<Heat_distort> src);
  Str get_avaliable_first_anim() const;

public:
  Emit_wnd();
  ~Emit_wnd() = default;
  inline void draw(Image& dst) const override {}
  inline void update(const Delta_time dt) override {}
  void imgui_exec() override;
};
