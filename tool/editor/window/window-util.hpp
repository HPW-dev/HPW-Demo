#pragma once
#include <functional>
#include "util/macro.hpp"
#include "util/math/vec.hpp"
#include "util/vector-types.hpp"
#include "util/mempool.hpp"
#include "util/str.hpp"

class Image;
class Entity;
class Anim;

/// рисует все кадры поворотных анимаций
void draw_frames(Pool_ptr(Entity) entity, Image& dst, const Vec pos);
/// безопасное получении анимации из anim_ctx
Anim* get_anim();

/// для удобного редактирования флагов imgui
class Flag_editor {
public:
  using get_flag_pf = std::function<bool ()>;
  using set_flag_pf = std::function<void (bool)>;

  Flag_editor() = default;
  ~Flag_editor();
  void emplace(get_flag_pf&& get_f, set_flag_pf&& set_f, CN<Str> name);
  void accept();

private:
  struct Flag {
    get_flag_pf get_f {};
    set_flag_pf set_f {};
    Str name {};
    bool value {};
  };

  Vector<Flag> m_flags {};
  bool m_accepted {false};
}; // Flag_editor
