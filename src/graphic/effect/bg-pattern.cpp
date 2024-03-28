#include <omp.h>
#include <array>
#include <cassert>
#include <algorithm>
#include "bg-pattern.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/graphic-util.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/util/rotation.hpp"
#include "graphic/sprite/sprite.hpp"
#include "util/math/random.hpp"
#include "util/math/mat.hpp"
#include "util/math/vec.hpp"
#include "util/math/vec-util.hpp"
#include "game/util/game-util.hpp"

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

  inline Elem get_safe(int x, int y) const {
    x = std::clamp<int>(x, 0, m_x-1);
    y = std::clamp<int>(y, 0, m_y-1);
    return m_grid[y * scast<int>(m_x) + x];
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

void bgp_bit_1(Image& dst, const int bg_state) {
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
} // bgp_bit_1

void bgp_bit_2(Image& dst, const int bg_state) {
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

void bgp_bit_3(Image& dst, const int bg_state) {
  int v = bg_state;
  cfor (y, dst.Y)
  cfor (x, dst.X) {
    int pix = (x - v) & (y + v);
    pix += v;
    dst(x, y) = pix;
  }
}

inline Pal8 rnd_color_fast() { return Pal8::from_real(rndr_fast(), rndu_fast() & 1); }

void bgp_pinterest_1(Image& dst, const int bg_state) {
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
} // bgp_pinterest_1

void bgp_rain_waves(Image& dst, const int bg_state) {
  static Waves waves(0.6);
  waves.update_size(dst.X, dst.Y);
  if (bg_state % 1'200 == 0)
    waves.clean();
  waves.make_noise(7.5, 1);
  waves.update();
  waves.draw(dst);
} // bgp_rain_waves

void bgp_line_waves(Image& dst, const int bg_state) {
  static Waves waves(2.0);
  waves.update_size(dst.X, dst.Y);
  if (bg_state % 1'200 == 0)
    waves.clean();
  if (bg_state % 15 == 0)
    waves.make_random_lines(1.75, 300, 5);
  waves.update();
  waves.draw(dst);
} // bgp_line_waves

inline void bg_pattern_7_line(const Vec center, const real deg,
const Pal8 color, Sprite& dst) {
  Vec a (center.x, center.y - 700);
  Vec b (center.x, center.y + 700);
  a = rotate_deg(center, a, deg);
  b = rotate_deg(center, b, deg);
  Vec offset(1, 0); // будет делать линию жирнее
  offset = rotate_deg({}, offset, deg);
  // нарисовать толстую линию
  constexpr int line_bold = 20; // ширина линии
  for (int i = -line_bold; i < line_bold; i += 3) {
    draw_line(*dst.get_image(), a + (offset * i), b + (offset * i), color);
    draw_line(*dst.get_mask(), a + (offset * i), b + (offset * i), Pal8::mask_visible);
  }
}

void bgp_rotated_lines(Image& dst, const int bg_state) {
  // красный фон
  cauto bg_color = Pal8::from_real(0.25 + std::fmod(bg_state * 0.003, 0.75), true);
  dst.fill(bg_color);

  // нарисовать несколько отрезков в буффере
  Sprite lines(dst.X, dst.Y);
  lines.get_image()->fill(Pal8::black);
  lines.get_mask()->fill(Pal8::mask_invisible);
  constexpr uint max_lines = 5;
  #pragma omp parallel for
  cfor (line_id, max_lines) {
    cauto center = Vec(
      (bg_state + line_id * 516262) % dst.X,
      (bg_state + line_id * 15125126) % dst.Y
    );
    const real deg = ring_deg(bg_state + line_id * 35325326);
    cauto color (Pal8::from_real(0.5 + std::fmod((((bg_state * 0.2) + (line_id * 1.65))), 0.5)));
    bg_pattern_7_line(center, deg, color, lines);
  }

  // вставить отрезки с буффера со скроллингом и тенью
  auto& mask = *lines.get_mask();
  cauto mask_bak = Image(mask);
  cauto shadow_len = 8;
  #pragma omp parallel for simd collapse(2)
  cfor (y, dst.Y)
  cfor (x, dst.X) {
    if (cauto pixel = mask_bak(x, y); pixel == Pal8::mask_visible)
      cfor (shadow, shadow_len)
        mask.set(x + shadow, y, Pal8::mask_visible, {});
  }

  insert(dst, lines);
} // bgp_rotated_lines

void bgp_random_lines_1(Image& dst, const int bg_state) {
  cauto bg_color = Pal8::from_real(0.25, true);
  dst.fill(bg_color);

  cauto max_lines = 200;
  #pragma omp parallel for
  cfor (i, max_lines) {
    const Vec a((bg_state + i * 1421515) % dst.X, 0);
    const Vec b((bg_state + i * 2346632) % dst.X, dst.Y);
    draw_line(dst, a, b, Pal8::white);
  }
} // bgp_random_lines_1

void bgp_random_lines_2(Image& dst, const int bg_state) {
  cauto bg_color = Pal8::from_real(0.25, true);
  dst.fill(bg_color);

  cauto max_lines = 4;
  cfor (i, max_lines) {
    const Vec a((bg_state + i * 5519204) % dst.X, 0);
    const Vec b((bg_state * 2 + i * 1492945) % dst.X, dst.Y);
    const Vec a1(0, (bg_state * 3 + i * 21441335) % dst.Y);
    const Vec b1(dst.X, (bg_state * 4 + i * 412499) % dst.Y);
    draw_line(dst, a, b, Pal8::white);
    draw_line(dst, a1, b1, Pal8::white);
  }
} // bgp_random_lines_2

void bgp_labyrinth_1(Image& dst, const int bg_state) {
  cauto state = bg_state / 95; // замедляет анимацию

  // узоры
  constexpr auto block_sz = 11u;
  Vector<Image> blocks;
  { // block gen
    Image block(block_sz, block_sz);
    // горизонтальная прямая
    block.fill(Pal8::black);
    cfor (x, block_sz)
      block(x, block.Y / 2) = Pal8::white;
    blocks.push_back(block);
    // вернтикальная прямая
    blocks.push_back( rotate90(block) );
    // крест
    block.fill(Pal8::black);
    cfor (i, block_sz) {
      block(i, block.Y / 2) = Pal8::white;
      block(block.X / 2, i) = Pal8::white;
    }
    blocks.push_back(block);
    // угол L
    block.fill(Pal8::black);
    cfor (i, block_sz / 2 + 1) {
      block(i, block.Y / 2) = Pal8::white;
      block(block.X / 2, i) = Pal8::white;
    }
    blocks.push_back(block);
    // повороты углов
    blocks.push_back( rotate90(block, 1) );
    blocks.push_back( rotate90(block, 2) );
    blocks.push_back( rotate90(block, 3) );
  } // block gen

  // генерация узора
  constexpr auto map_x = 64u;
  constexpr auto map_y = 56u;
  std::array<std::size_t, map_x * map_y> map;
  for (uint i = 0; nauto elem: map) {
    elem = (i ^ ((i % block_sz) * state)) % blocks.size();
    ++i;
  }
  
  // отрисовка
  #pragma omp parallel for schedule(static, 4)
  cfor (y, map_y)
  cfor (x, map_x) {
    cauto id = map[y * map_x + x];
    cauto pos = Vec(x * block_sz, y * block_sz);
    insert(dst, blocks[id], pos);
  }
} // bgp_labyrinth_1

void bgp_labyrinth_2(Image& dst, const int bg_state) {
  // узоры
  constexpr auto block_sz = 11u;
  static Vector<Image> blocks;
  static bool firt_init = true;
  if (firt_init) { // block gen
    Image block(block_sz, block_sz);
    // угол L
    block.fill(Pal8::black);
    cfor (i, block_sz / 2 + 1) {
      block(i, block.Y / 2) = Pal8::white;
      block(block.X / 2, i) = Pal8::white;
    }
    blocks.push_back(block);
    // повороты углов
    blocks.push_back( rotate90(block, 1) );
    blocks.push_back( rotate90(block, 2) );
    blocks.push_back( rotate90(block, 3) );
  } // block gen

  // генерация узора
  constexpr auto map_x = 64u;
  constexpr auto map_y = 56u;
  static std::array<std::size_t, map_x * map_y> map;
  if (firt_init)
    for (uint i = 0; nauto elem: map)
      elem = ((++i) % block_sz) % blocks.size();
  
  // случайно поменять несколько блоков узора
  cfor (_, 50)
    map[rndu(map.size() - 1)] = rndu(blocks.size() - 1);

  // отрисовка
  #pragma omp parallel for schedule(static, 4)
  cfor (y, map_y)
  cfor (x, map_x) {
    cauto id = map[y * map_x + x];
    cauto pos = Vec(x * block_sz, y * block_sz);
    insert(dst, blocks[id], pos);
  }

  firt_init = false;
} // bgp_labyrinth_2

/// 3D точка
struct Vec3 { real x {}, y {}, z {}; };

/// рисует 3D точку
inline void draw_3d_point(Image& dst, const Vec3 point, const Vec offset,
const Pal8 color, const real focal_len = 1.0) {
  const Vec pos (
    safe_div(point.x, point.z) * focal_len,
    safe_div(point.y, point.z) * focal_len
  );
  dst.set(offset.x + pos.x, offset.y + pos.y, color);
}

/// рисует 3D точку в изометрии
inline void draw_3d_point_isometry(Image& dst, const Vec3 point, const Vec offset,
const Pal8 color, const real scale = 1.0) {
  const Vec pos (
    (point.x * 2.1 + point.z * 0.15) * 0.75,
    point.y - point.z
  );
  cfor (y, 3)
  cfor (x, 3)
    dst.set<&blend_max>(
      offset.x + pos.x * scale + x - 1,
      offset.y + pos.y * scale + y - 1,
      color, {});
}

Vec3 rotate(const Vec3 src, const real pitch, const real roll, const real yaw) {
  cauto cosa = std::cos(yaw);
  cauto sina = std::sin(yaw);

  cauto cosb = std::cos(pitch);
  cauto sinb = std::sin(pitch);

  cauto cosc = std::cos(roll);
  cauto sinc = std::sin(roll);

  cauto Axx = cosa*cosb;
  cauto Axy = cosa*sinb*sinc - sina*cosc;
  cauto Axz = cosa*sinb*cosc + sina*sinc;

  cauto Ayx = sina*cosb;
  cauto Ayy = sina*sinb*sinc + cosa*cosc;
  cauto Ayz = sina*sinb*cosc - cosa*sinc;

  cauto Azx = -sinb;
  cauto Azy = cosb*sinc;
  cauto Azz = cosb*cosc;

  return Vec3 {
    .x = Axx * src.x + Axy * src.y + Axz * src.z,
    .y = Ayx * src.x + Ayy * src.y + Ayz * src.z,
    .z = Azx * src.x + Azy * src.y + Azz * src.z};
} // rotate

void bgp_3d_atomar_cube(Image& dst, const int bg_state) {
  const real rot_speed = bg_state / 240.0;
  cauto center = center_point(dst);

  dst.fill(Pal8::black);
  for (real z = -1; z < 1; z += 0.1)
  for (real y = -1; y < 1; y += 0.1)
  for (real x = -1; x < 1; x += 0.1) {
    Vec3 pos {.x = x, .y = y, .z = z};
    pos = rotate(pos, rot_speed, 0, 0);
    // делать дальние точки темнее
    auto color = Pal8::from_real( 1.0 - ((pos.z + 0.5) * 0.33333) );
    draw_3d_point_isometry(dst, pos, center, color, 80.0);
  }
}
