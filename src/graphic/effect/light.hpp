#pragma once
#include <cstdint>
#include "graphic/image/color-blend.hpp"
#include "util/math/vec.hpp"
#include "graphic/image/image-fwd.hpp"

enum class Light_quality {
  disabled = 0, // играть без вспышек
  low, // рисовать только звёздочки
  medium, // обычный ровный градиент c премапом
  high // каждый кадр генерить градиент и полоски на нём
};

// Вспышки света
class Light final {
public:
  // если вспышка превывает этот радиус, то загорается весь экран
  constx real MAX_LIGHT_RADIUS {400};
  // из-за гамма-коррекции свет может быть тусклее, поэтому он усиливается
  constx real LIGHT_AMPLIFY = 2.3333333f;
  constx real GAMMA_CORR = 2.2f;

  static void make_lines(Image& dst); // создаёт полоски на сфере свечения
  
private:
  real cur_duration {};
  real max_duration {};

  void draw_light_sphere(Image& dst, const Vec pos, real tmp_radius) const;
  void draw_fullscreen_blink(Image& dst) const;
  void draw_light_star(Image& dst, const Vec pos, const real tmp_radius) const;
  [[nodiscard]] real get_new_radius() const; // получить радиус с учётом флагов
  [[nodiscard]] real effect_ratio() const; // вернет значение в 0..1 зависимости от оставшегося времени и флагов 

public:
  struct Flags {
    bool repeat                 : 1 {};
    bool decrease_radius        : 1 {true};
    bool invert_decrease_radius : 1 {};
    bool random_radius          : 1 {true};
    bool star                   : 1 {true}; // рисовать во вспышке обычную звёздочку
    bool star_diagonal          : 1 {true}; // рисовать во вспышке диагональную звёздочку
    bool no_sphere              : 1 {}; // не рисовать круглую вспышку
  };

  blend_pf bf {&blend_max};
  blend_pf bf_star {&blend_max}; // режим смешивания при отрисовке звезды
  std::size_t radius {};
  Flags flags {};

  Light() = default;
  ~Light() = default;
  void reset();
  bool update(const Delta_time dt); // ret 0 if end of effect
  void draw(Image& dst, const Vec pos) const;
  void set_duration(real new_duration);
  inline real& get_cur_duration() { return cur_duration; }
  inline real& get_max_duration() { return max_duration; }
}; // Light

// генерирует пререндереные сферы для вспышек
void cache_light_spheres();
