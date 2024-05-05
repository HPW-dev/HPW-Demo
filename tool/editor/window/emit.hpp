#pragma once
#include <unordered_map>
#include "window.hpp"
#include "util/macro.hpp"
#include "util/str.hpp"

class Heat_distort;

enum class spawner_t {
  statical = 0,
  directed,
  spray,
};

inline std::unordered_map<spawner_t, Str> spawner_type_table {
  {spawner_t::statical, "statical"},
  {spawner_t::directed, "directed"},
  {spawner_t::spray, "spray"},
};

Strs spawner_type_names();

// настройки спавнера частиц
class Emit_wnd: public Window {
  spawner_t type {spawner_t::statical};

  void draw_spawner_types();
  void draw_anim_list();
  void make_new_anim();
  void delete_anim_by_name(CN<Str> name);
  void select_anim_by_name(CN<Str> name);
  void draw_entity_flags();
  void heat_distort_edit();
  void heat_distort_flags_edit(Heat_distort& heat_distort);
  void light_edit();
  void reset();
  void save_to_clipboard(CN<Heat_distort> src);
  Str get_avaliable_first_anim() const;

public:
  Emit_wnd();
  ~Emit_wnd() = default;
  inline void draw(Image& dst) const override {}
  inline void update(double dt) override {}
  void imgui_exec() override;
};
