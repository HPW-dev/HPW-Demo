#include <omp.h>
#include <cassert>
#include <algorithm>
#include "bg-pattern.hpp"
#include "graphic/image/image.hpp"
#include "util/math/random.hpp"
#include "util/math/vec.hpp"
#include "util/math/vec-util.hpp"

/// симуляция волн
class Waves final {
  struct Elem {
    real value {};
    real velocity {};
  };
  Vector<Elem> m_grid {};
  std::size_t m_x {};
  std::size_t m_y {};
  real m_amp {1}; /// усиление волны

  inline Elem& get(const std::size_t x, const std::size_t y)
    { return m_grid[y * m_x + x]; }

  inline Elem get_safe(const int x, const int y) const {
    if (x >= 0 && x < scast<int>(m_x) && y >= 0 && y < scast<int>(m_y))
      return m_grid[y * scast<int>(m_x) + x];
    return {};
  }

  inline void set_safe(const int x, const int y, const real value) {
    if (x >= 0 && x < scast<int>(m_x) && y >= 0 && y < scast<int>(m_y))
      m_grid[y * scast<int>(m_x) + x].value = value;
  }

public:
  Waves() = default;
  ~Waves() = default;

  inline Waves(const real amp): m_amp{amp} {}

  inline void clean() {
    for (nauto it: m_grid)
      it.value = it.velocity = 0;
  }

  inline void update_size(const std::size_t X, const std::size_t Y) {
    m_x = X;
    m_y = Y;
    m_grid.resize(X * Y);
  }

  /** источники колебаний в случайных местах
  * @param rnd_power на сколько сильное возмущение создаётся
  * @param freq как часто за раз делать возмущения */
  inline void make_noise(const real rnd_power, const std::size_t freq) {
    assert( !m_grid.empty());

    cfor (_, freq)
      m_grid[rndu_fast(m_grid.size()-1)].value += rndr_fast(-rnd_power, rnd_power);
  }

  inline void make_random_lines(const real rnd_power,
  const std::size_t rnd_len, const std::size_t freq) {
    cfor (_, freq) {
      const bool is_horizeontal = rndu_fast() % 2 == 0;
      const Vec pos(
        rndr_fast(-scast<int>(rnd_len), m_x),
        rndr_fast(-scast<int>(rnd_len), m_y) );
      cauto val = rndr_fast(-rnd_power, rnd_power);
      const int len = rndu_fast(rnd_len);

      if (is_horizeontal) {
        for (int x = 0; x < len; ++x)
          set_safe(pos.x + x, pos.y, val);
      } else {
        for (int y = 0; y < len; ++y)
          set_safe(pos.x, pos.y + y, val);
      }
    } // for freq
  }

  inline void overlay(CN<Image> dst) {
    assert(scast<std::size_t>(dst.size) <= m_grid.size());

    cfor (i, dst.size)
      m_grid[i].value = dst[i].to_real();
  }

  inline void update() {
    // изменить скорость колебания волны
    #pragma omp parallel for simd collapse(2)
    cfor (y, m_y)
    cfor (x, m_x) {
      constexpr real sin_of_45_deg = 0.70710678118;
      cauto A = get_safe(x-1, y-1).value * sin_of_45_deg;
      cauto B = get_safe(x+0, y-1).value;
      cauto C = get_safe(x+1, y-1).value * sin_of_45_deg;
      cauto D = get_safe(x-1, y+0).value;
      nauto P = get     (x+0, y+0);
      cauto E = get_safe(x+1, y+0).value;
      cauto F = get_safe(x-1, y+1).value * sin_of_45_deg;
      cauto G = get_safe(x+0, y+1).value;
      cauto K = get_safe(x+1, y+1).value * sin_of_45_deg;
      constexpr real mul = 1.0 / 8.0;
      cauto offset = ((A+B+C+D+E+F+G+K) * mul) - P.value;
      P.velocity += offset * m_amp;
    }

    // распространить волну
    cfor (i, m_grid.size())
      m_grid[i].value += m_grid[i].velocity;
  } // update

  inline void draw(Image& dst) const {
    assert(scast<std::size_t>(dst.size) <= m_grid.size());

    #pragma omp parallel for simd
    cfor (i, dst.size) {
      cauto normalized = (std::clamp<real>(m_grid[i].value, -1, 1) + 1.0) * 0.5;
      dst[i] = Pal8::from_real(normalized);
    }
  }
}; // Waves

void bg_pattern_1(Image& dst, const int bg_state) {
  // нарисовать фон
  int v = bg_state;
  cfor (y, dst.Y)
  cfor (x, dst.X) {
    int pix = ((x + v) >> 2) * ((y - v) >> 2);
    int tmp = pix << 4;
    tmp |= v;
    pix &= v;
    pix ^= tmp;
    int pix2 = (x + v) ^ (y - v);
    tmp = pix2 >> 2;
    tmp |= v;
    pix2 &= v;
    pix2 ^= tmp;
    pix &= pix2;
    dst(x, y) = pix;
  }
}

void bg_pattern_2(Image& dst, const int bg_state) {
  int v = bg_state;
  cfor (y, dst.Y)
  cfor (x, dst.X) {
    int pix = (x + v) ^ (y + v);
    pix |= v;
    pix >>= 4;
    pix <<= 4;
    dst(x, y) = pix;
  }
}

void bg_pattern_3(Image& dst, const int bg_state) {
  int v = bg_state;
  cfor (y, dst.Y)
  cfor (x, dst.X) {
    int pix = (x - v) & (y + v);
    pix += v;
    dst(x, y) = pix;
  }
}

inline Pal8 rnd_color_fast() { return Pal8::from_real(rndr_fast(), rndu_fast() & 1); }

void bg_pattern_4(Image& dst, const int bg_state) {
  constx auto MX = 12u;
  constx auto ICONS = 5u;

  cfor (y, dst.Y) {
    cfor(x, dst.X)
      dst.set(x, y * 3, Pal8::black, {});
    int pos = 0;
    cfor (x, MX) {
      Vector<Pal8> left_colors(ICONS);
      Vector<Pal8> right_colors(ICONS);
      std::generate(left_colors.begin(), left_colors.end(), [=]{
        return rnd_color_fast().val - bg_state; });
      std::generate(right_colors.begin(), right_colors.end(), [=]{
        return rnd_color_fast().val + bg_state; });
      cauto tmp_pos = pos;
      cfor (y2, 2) {
        pos = tmp_pos;
        dst.set(pos, y*3 + y2 + 1, Pal8::black, {}); ++pos;
        dst.set(pos, y*3 + y2 + 1, Pal8::black, {}); ++pos;
        cfor (icon, ICONS) {
          dst.set(pos, y*3 + y2 + 1, Pal8::black, {}); ++pos;
          auto color = left_colors[icon];
          dst.set(pos, y*3 + y2 + 1, color, {}); ++pos;
          dst.set(pos, y*3 + y2 + 1, color, {}); ++pos;
          dst.set(pos, y*3 + y2 + 1, color, {}); ++pos;
          dst.set(pos, y*3 + y2 + 1, color, {}); ++pos;
        }
        dst.set(pos, y*3 + y2 + 1, Pal8::black, {}); ++pos;
        cfor (icon, ICONS) {
          dst.set(pos, y*3 + y2 + 1, Pal8::black, {}); ++pos;
          cauto color = right_colors[icon];
          dst.set(pos, y*3 + y2 + 1, color, {}); ++pos;
          dst.set(pos, y*3 + y2 + 1, color, {}); ++pos;
        }
      } // for y2 -> 2
    } // for x -> MX
  }
} // bg_pattern_4

void bg_pattern_5(Image& dst, const int bg_state) {
  static Waves waves(0.6);
  waves.update_size(dst.X, dst.Y);
  if (bg_state % 1'200 == 0)
    waves.clean();
  waves.make_noise(7.5, 1);
  waves.update();
  waves.draw(dst);
} // bg_pattern_5

void bg_pattern_6(Image& dst, const int bg_state) {
  static Waves waves(2.0);
  waves.update_size(dst.X, dst.Y);
  if (bg_state % 1'200 == 0)
    waves.clean();
  if (bg_state % 15 == 0)
    waves.make_random_lines(1.75, 300, 5);
  waves.update();
  waves.draw(dst);
} // bg_pattern_6
