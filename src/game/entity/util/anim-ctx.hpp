#pragma once
#include "util/mempool.hpp"
#include "util/math/vec.hpp"
#include "graphic/image/color-blend.hpp"

class Anim;
class Frame;
class Image;
class Entity;
class Direct;
class Hitbox;

// Управляет анимацией объекта
class Anim_ctx final {
public:
  blend_pf blend_f {&blend_past}; // режим наложения основной картинки
  blend_pf contour_bf {&blend_158}; // режим наложения контура
  
  Anim_ctx() = default;
  ~Anim_ctx() = default;
  Anim_ctx(CP<Anim> new_anim);

  void update(const Delta_time dt, Entity &entity);
  void draw(Image& dst, CN<Entity> entity, const Vec offset);
  void set_cur_frame(std::size_t num);
  void set_last_frame();
  void set_anim(CP<Anim> new_anim);
  void set_speed_scale(real new_scale);

  void set_contour(CP<Anim> val);
  inline CP<Anim> get_contour() const { return m_contour; }

  void randomize_cur_frame_safe();

  // получить индекс текущего кадра
  inline cnauto get_cur_frame_idx() const { return m_frame_idx; }
  CP<Frame> get_cur_frame() const;
  inline cnauto get_anim() const { return m_anim; }
  // узнать скорость анимации
  inline cnauto get_speed_scale() const { return m_speed_scale; }
  // переключить на следыдущий кадр
  inline void next_frame() { set_cur_frame(m_frame_idx + 1); }
  // переключить на предующий кадр
  inline void prev_frame() { set_cur_frame(m_frame_idx - 1); }
  // задать статичный угол (юзается флагом .m_fixed_deg)
  void set_default_deg(real deg);
  inline real get_default_deg() const { return m_fixed_deg; }
  // координаты, в которых объект был нарисован
  inline Vec get_drawed_pos() const { return m_drawed_pos; }
  // получение угла поворота с учётом флагов
  real get_degree_with_flags(real src, CN<Entity> entity) const;

private:
  CP<Anim> m_anim {}; // анимация с банка
  CP<Anim> m_contour {}; // контур для выделения
  real m_frame_timer {}; // таймер длительности кадра
  real m_speed_scale {1.0}; // скорость воспоризведения анимации
  real m_fixed_deg {}; // поворот по умолчанию, для флага m_fixed_deg
  std::size_t m_frame_idx {}; // текущий кадр в анимации
  std::size_t m_prev_frame_idx {}; // предыдущий кадр
  mutable Vec m_draw_pos {}; // место, где сейчас нарисовался объект
  mutable Vec m_old_draw_pos {}; // место, где был нарисован объект
  // место, где был нарисован объект с интерполяцией
  mutable Vec m_old_interp_pos {};
  mutable Vec m_drawed_pos {}; // здесь объект был нарисован

  Vec get_interpolated_pos() const;
  void next_frame_idx(Entity &entity);
  void draw_contour(Image& dst, const Vec offset, real degree) const;
  // обратный порядок кадров
  void goto_prev_frame(Entity &entity);
  // сделать следующий кадр случайный
  void goto_rnd_frame();
  // режим обратного воспроизведения кадров
  void return_back_frame(Entity &entity);
  // запустить анимацию заново
  void reset_animation(Entity &entity);
}; // Anim_ctx
