#include <omp.h>
#include <algorithm>
#include <unordered_map>
#include <utility>
#include <cmath>
#include <cassert>
#include <cstdint>
#include "light.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/graphic-util.hpp"
#include "util/math/vec.hpp"
#include "util/math/vec-util.hpp"
#include "util/math/mat.hpp"
#include "util/math/random.hpp"
#include "game/core/graphic.hpp"
#include "game/util/sync.hpp"

std::unordered_map<int, Image> cached_spheres {}; // для оптимизации с пререндером вспышек
inline constexpr int cache_spheres_steps = 3; // чем меньше, тем больше пререндеров
bool cache_spheres_once {true};

void cache_light_spheres() {
  // вызывать это только 1 раз
  return_if (!cache_spheres_once);
  cache_spheres_once = false;

  constexpr int count = Light::MAX_LIGHT_RADIUS / cache_spheres_steps;
  static_assert(count > 0);
  static_assert(count < 500);

  #pragma omp parallel for schedule(dynamic)
  cfor (i, count) {
    cauto R = (i + 1) * cache_spheres_steps;
    // нарисовать сферический градиент на чёрной картинке
    auto sphere_sz = std::ceil(R * 2);
    auto center = Vec(R, R);
    Image sphere(sphere_sz, sphere_sz);
    cfor (y, sphere_sz)
    cfor (x, sphere_sz) {
      auto dist = distance(center, Vec(x, y));
      auto ratio = 1.0 - (dist / R);
      sphere(x, y) = Pal8::from_real(ratio);
    }
    Light::make_lines(sphere);
    #pragma omp critical (write_to_cached_spheres)
    { cached_spheres[i] = std::move(sphere); }
  }
}

void draw_cached_sphere(Image& dst, const real R, const Vec pos, blend_pf bf) {
  assert(dst);
  return_if(R <= 0);
  // TODO оптимизация выхода за dst

  // сгенерить пререндеры, если их нету
  cache_light_spheres();

  assert(cache_spheres_steps > 0);
  int index = R / cache_spheres_steps;
  index = std::clamp<int>(index, 0, cached_spheres.size() - 1);
  cnauto sphere = cached_spheres.at(index);
  cauto offset = pos - center_point(sphere);
  insert(dst, sphere, offset, bf);
}

void Light::reset() { cur_duration = max_duration; }

bool Light::update(const Delta_time dt) {
  return_if(dt <= 0, cur_duration > 0);
  cur_duration -= dt;

  // перезапуск, если бесконечная вспышка
  if (flags.repeat && cur_duration <= 0)
    cur_duration = max_duration;

  return cur_duration > 0; // если таймер не кончился, то вспышка ещё живая
}

void Light::draw(Image& dst, const Vec pos) const {
  assert(dst);
  // выйти, если таймер вспышки кончился и она не бесконечная
  return_if (cur_duration <= 0 && !flags.repeat);

  cauto tmp_radius = get_new_radius();

  if (graphic::light_quality != Light_quality::low) {
    // когда вспышка слишком больгого размера, сверкает весь экран
    if (tmp_radius >= MAX_LIGHT_RADIUS) {
      draw_fullscreen_blink(dst);
      return;
    }

    if (!flags.no_sphere)
      draw_light_sphere(dst, pos, tmp_radius);
  }

  draw_light_star(dst, pos, tmp_radius);
} // draw

void Light::set_duration(real new_duration) {
  assert(new_duration > 0);
  cur_duration = max_duration = new_duration;
}

void Light::draw_light_sphere(Image& dst, const Vec pos, const real tmp_radius) const {
  if (graphic::light_quality == Light_quality::high) {
    return_if(tmp_radius <= 0);
    // нарисовать сферический градиент на чёрной картинке
    auto sphere_sz = std::ceil(tmp_radius * 2);
    auto center = Vec(tmp_radius, tmp_radius);
    Image sphere(sphere_sz, sphere_sz);
    cfor (y, sphere_sz)
    cfor (x, sphere_sz) {
      auto dist = distance(center, Vec(x, y));
      auto ratio = 1.0 - (dist / tmp_radius);
      sphere(x, y) = Pal8::from_real(ratio);
    }
    make_lines(sphere);

    // нарисовать свечение вспышки
    auto offset = pos - center;
    insert(dst, sphere, offset, bf);
    return;
  }
  
  // medium:
  // мигать при лагах
  return_if (graphic::render_lag && ((graphic::frame_count & 1) == 0));
  draw_cached_sphere(dst, tmp_radius, pos, bf);
} // draw_light_sphere

void Light::draw_fullscreen_blink(Image& dst) const {
  if (graphic::light_quality == Light_quality::medium) {
    // мигать при лагах
    return_if (graphic::render_lag && ((graphic::frame_count & 1) == 0));
    dst.fill(Pal8::white);
    return;
  }

  // high
  for (nauto pix: dst)
    pix = bf(Pal8::from_real(safe_div(cur_duration, max_duration)), pix, {});
}

void Light::draw_light_star(Image& dst, const Vec pos, const real tmp_radius) const {
  return_if(cur_duration <= 0);
  assert(max_duration > 0);
  // на низких настройках можно мигать при тормозах
  return_if (graphic::light_quality == Light_quality::low &&
    graphic::render_lag && ((graphic::frame_count & 1) == 0));

  cauto ratio = scast<real>(cur_duration) / max_duration;
  auto range = tmp_radius * 1.5;
  cauto ceiled_range = std::ceil(range);
  return_if (ceiled_range < 1);

  cauto floor_pos = floor(pos);
  const int optional = ratio * 255.0; // для бленда звезды
  
  // TODO не рисовать за пределами

  // вертикальный и горизонтальный луч звезды
  if (flags.star) {
    cfor (i, ceiled_range) {
      cauto color = Pal8::from_real((ceiled_range - i) / ceiled_range);
      dst.set(floor_pos.x+i, floor_pos.y, color, bf_star, optional);
      dst.set(floor_pos.x-i, floor_pos.y, color, bf_star, optional);
      dst.set(floor_pos.x, floor_pos.y+i, color, bf_star, optional);
      dst.set(floor_pos.x, floor_pos.y-i, color, bf_star, optional);
    }
  }

  // диагональные лучи звезды
  return_if( !flags.star_diagonal);
  cauto ceiled_range_diagonal = std::ceil(ceiled_range * 0.5); // диагональные короче
  cfor (i, ceiled_range_diagonal) {
    cauto color = Pal8::from_real((ceiled_range_diagonal - i) / ceiled_range_diagonal);
    dst.set(floor_pos.x-i, floor_pos.y-i, color, bf_star, optional);
    dst.set(floor_pos.x+i, floor_pos.y-i, color, bf_star, optional);
    dst.set(floor_pos.x-i, floor_pos.y+i, color, bf_star, optional);
    dst.set(floor_pos.x+i, floor_pos.y+i, color, bf_star, optional);
  }
}

void Light::make_lines(Image& dst) {
  cfor (y, dst.Y) {
    Pal8 for_sub = Pal8::from_real(rndr_fast(0, 0.17));
    cfor (x, dst.X) {
      nauto dst_pix = dst(x, y);
      dst_pix = blend_sub_safe(for_sub, dst_pix);
    }
  }
}

real Light::get_new_radius() const {
  cauto ratio = safe_div(scast<real>(cur_duration), max_duration);
  auto ret = radius;
  if (flags.random_radius)
    ret = rndr_fast(0, radius * ratio);

  if (flags.decrease_radius) {
    if (flags.invert_decrease_radius)
      ret *= (1.0 - ratio);
    else
      ret *= ratio;
  }

  return ret;
}
