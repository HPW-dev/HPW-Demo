#pragma once
#include "graphic/image/color-blend.hpp"
#include "util/mempool.hpp"
#include "util/math/vec.hpp"

class Anim;
class Entity;
class Direct;
class Frame;
class Image;
class Hitbox;

// Управляет анимацией объекта
class Anim_ctx final {
public:
  blend_pf blend_f {&blend_past}; // режим наложения основной картинки
  blend_pf contour_bf {&blend_158}; // режим наложения контура
  
  Anim_ctx() = default;
  Anim_ctx(CP<Anim> new_anim);
  ~Anim_ctx() = default;

  void update(const Delta_time dt, Entity &entity);
  void draw(Image& dst, CN<Entity> entity, const Vec offset);
  void set_cur_frame(std::size_t num);
  void set_last_frame();
  void set_anim(CP<Anim> new_anim);
  void set_speed_scale(real new_scale);

  void set_contour(CP<Anim> val);
  inline CP<Anim> get_contour() const { return contour; }

  void randomize_cur_frame_safe();

  // получить индекс текущего кадра
  inline cnauto get_cur_frame_idx() const { return frame_idx; }
  CP<Frame> get_cur_frame() const;
  inline cnauto get_anim() const { return anim; }
  // узнать скорость анимации
  inline cnauto get_speed_scale() const { return speed_scale; }
  // переключить на следыдущий кадр
  inline void next_frame() { set_cur_frame(frame_idx + 1); }
  // переключить на предующий кадр
  inline void prev_frame() { set_cur_frame(frame_idx - 1); }
  // получить хитбокс анимации
  CP<Hitbox> get_hitbox(real degree, CN<Entity> entity) const;
  // назначить новых хитбокс
  void update_hitbox(CN<Pool_ptr(Hitbox)> _hitbox);
  // задать статичный угол (юзается флагом .fixed_deg)
  void set_default_deg(real deg);
  inline real get_default_deg() const { return fixed_deg; }
  // координаты, в которых объект был нарисован
  inline Vec get_draw_pos() const { return m_draw_pos; }

private:
  CP<Anim> anim {}; // анимация с банка
  CP<Anim> contour {}; // контур для выделения
  real frame_timer {}; // таймер длительности кадра
  real speed_scale {1.0}; // скорость воспоризведения анимации
  real fixed_deg {}; // поворот по умолчанию, для флага fixed_deg
  std::size_t frame_idx {}; // текущий кадр в анимации
  std::size_t prev_frame_idx {}; // предыдущий кадр
  mutable Vec draw_pos {}; // место, где сейчас нарисовался объект
  mutable Vec old_draw_pos {}; // место, где был нарисован объект
  // место, где был нарисован объект с интерполяцией
  mutable Vec old_interpolated_pos {};
  mutable Vec m_draw_pos {}; // здесь объект был нарисован

  // получение угла поворота с учётом флагов
  real get_degree_with_flags(real src, CN<Entity> entity) const;
  Vec get_interpolated_pos() const;
  void update_frame_idx(Entity &entity);
  void draw_contour(Image& dst, const Vec offset, real degree) const;
}; // Anim_ctx
