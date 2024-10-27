#include <omp.h>
#include <array>
#include <cmath>
#include <utility>
#include <cassert>
#include <functional>
#include <algorithm>
#include "bg-pattern-3.hpp"
#include "game/core/fonts.hpp"
#include "graphic/image/image.hpp"
#include "graphic/effect/dither.hpp"
#include "graphic/effect/noise.hpp"
#include "graphic/effect/blur.hpp"
#include "graphic/util/blend.hpp"
#include "graphic/util/resize.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/util/graphic-util.hpp"
#include "util/math/xorshift.hpp"
#include "util/math/vec-util.hpp"
#include "util/math/mat.hpp"
#include "util/math/random.hpp"
#include "util/rnd-table.hpp"

void bgp_liquid(Image& dst, const int bg_state) {
  cauto state = bg_state * 2;

  // сделать пятна
  #pragma omp parallel for simd collapse(2)
  cfor (y, dst.Y)
  cfor (x, dst.X) {
    real color =
      std::sin((x - y - state) / 140.f) / 2.f -
      std::cos((y +     state) / 100.f)     +
      std::cos((x + y + state) /  80.f) / 2.f -
      std::sin((y +     state) / 120.f);
    color += 1.f;
    color /= 2.f;
    color = color / (1.f + std::abs(color));
    dst(x, y) = Pal8::from_real(color);
  }

  fast_dither_bayer16x16_4bit(dst);
  to_red(dst);
}

void bgp_liquid_gray(Image& dst, const int bg_state) {
  auto state = bg_state * 2;

  // сделать пятна
  #pragma omp parallel for simd collapse(2)
  cfor (y, dst.Y)
  cfor (x, dst.X) {
    real color =
      std::sin((x - y - state) / 140.f) / 2.f -
      std::cos((y +     state) / 100.f)     +
      std::cos((x + y + state) /  80.f) / 2.f -
      std::sin((y +     state) / 120.f);
    color += 1.f;
    color /= 2.f;
    color = color / (1.f + std::abs(color));
    dst(x, y) = Pal8::from_real(color);
  }

  Image blured(dst);
  apply_invert(blured);

  // маска для блюра
  Image alpha_mask(dst.X, dst.Y);
  state *= 1.5;
  #pragma omp parallel for simd collapse(2)
  cfor (y, alpha_mask.Y)
  cfor (x, alpha_mask.X) {
    real color =
      std::sin((x + y + state) / 100.f) / 2.f +
      std::cos((y +     state) / 150.f)     -
      std::cos((x - y + state) /  50.f) / 3.f +
      std::sin((y +     state) / 120.f);
    color *= 3.f;
    color = color / (1.f + std::abs(color));
    alpha_mask(x, y) = Pal8::from_real(color);
  }

  accept_blend_mask_fast(dst, blured, alpha_mask);
}

void bgp_trajectory(Image& dst, const int bg_state) {
  dst.fill(Pal8::red);
  assert(graphic::font);

  xorshift128_state seed {
    .a = 0x2465,
    .b = 0xAAFF,
    .c = 0xBA32,
    .d = 0x202D,
  };

  Vector<Vec> points(20);
  for (rauto point: points)
    point = Vec(
      (xorshift128(seed) & bg_state) % dst.X,
      (xorshift128(seed) & bg_state) % dst.Y
    );

  cfor (i, points.size()-3)
    draw_spline(dst, points[i], points[i+1], points[i+2], points[i+3], Pal8::white);
  
  for (rauto point: points)
    graphic::font->draw(dst, point, sconv<utf32>(to_str(point, 0)));
}

void bgp_fading_grid(Image& dst, const int bg_state) {
  constx uint GRID_SZ = 32;
  cauto SPEED = bg_state * 4;
  cauto GRID_X = dst.X / GRID_SZ;
  cauto GRID_Y = dst.Y / GRID_SZ;
  xorshift128_state seed {
    .a = 0x2451,
    .b = 0xBEAF,
    .c = 0xDADA,
    .d = 0x2AAF,
  };

  struct Cell {
    bool is_red {};
    real gradient {};
  };
  Vector<Cell> cells(GRID_X * GRID_Y);
  for (rauto cell: cells) {
    cell.is_red = xorshift128(seed) % 2u;
    cell.gradient = std::fmod(xorshift128(seed) + SPEED, 1'000) / 1'000.f;
    // подъём и спад яркости
    if (cell.gradient > 0.5f)
      cell.gradient = 0.5f - (cell.gradient - 0.5f);
    cell.gradient *= 2.f;
  }

  #pragma omp parallel for simd collapse(2)
  cfor (y, GRID_Y)
  cfor (x, GRID_X) {
    cauto cell = cells[y * GRID_X + x];
    cauto is_red = cell.is_red;
    cauto gradient = cell.gradient;
    cfor (gy, GRID_SZ)
    cfor (gx, GRID_SZ)
      dst.set(x * GRID_SZ + gx, y * GRID_SZ + gy, Pal8::from_real(gradient, is_red));
  }
}

void bgp_fading_grid_dithered(Image& dst, const int bg_state) {
  constx uint GRID_SZ = 16;
  cauto SPEED = bg_state;
  cauto GRID_X = dst.X / GRID_SZ;
  cauto GRID_Y = dst.Y / GRID_SZ;
  xorshift128_state seed {
    .a = 0x2451,
    .b = 0xBEAF,
    .c = 0xDADA,
    .d = 0x2AAF,
  };

  struct Cell {
    bool is_red {};
    real gradient {};
  };
  Vector<Cell> cells(GRID_X * GRID_Y);
  for (rauto cell: cells) {
    cell.is_red = xorshift128(seed) % 2u;
    cell.gradient = std::fmod(xorshift128(seed) + SPEED, 1'000) / 1'000.f;
    // подъём и спад яркости
    if (cell.gradient > 0.5f)
      cell.gradient = 0.5f - (cell.gradient - 0.5f);
    cell.gradient *= 2.f;
  }

  #pragma omp parallel for simd collapse(2)
  cfor (y, GRID_Y)
  cfor (x, GRID_X) {
    cauto cell = cells[y * GRID_X + x];
    cauto is_red = cell.is_red;
    cauto gradient = cell.gradient;
    cfor (gy, GRID_SZ)
    cfor (gx, GRID_SZ)
      dst.set(x * GRID_SZ + gx, y * GRID_SZ + gy, Pal8::from_real(gradient, is_red));
  }

  dither_bayer16x16_1bit(dst);
}

void bgp_fading_grid_black(Image& dst, const int bg_state) {
  constx uint GRID_SZ = 16;
  cauto SPEED = bg_state * 3;
  cauto GRID_X = dst.X / GRID_SZ;
  cauto GRID_Y = dst.Y / GRID_SZ;
  xorshift128_state seed {
    .a = 0x2451,
    .b = 0xBEAF,
    .c = 0xDADA,
    .d = 0x2AAF,
  };

  Vector<real> cells(GRID_X * GRID_Y);
  for (rauto cell: cells) {
    cell = ((xorshift128(seed) + SPEED) % 1'000) / 1'000.f;
    // подъём и спад яркости
    if (cell > 0.5f)
      cell = 0.5f - (cell - 0.5f);
    cell *= 0.25f;
  }

  #pragma omp parallel for simd collapse(2)
  cfor (y, GRID_Y)
  cfor (x, GRID_X) {
    cfor (gy, GRID_SZ)
    cfor (gx, GRID_SZ)
      dst.set(x * GRID_SZ + gx, y * GRID_SZ + gy, Pal8::from_real(cells[y * GRID_X + x]));
  }
}

void bgp_fading_grid_red(Image& dst, const int bg_state) {
  constx uint GRID_SZ = 16;
  cauto SPEED = bg_state * 2;
  cauto GRID_X = dst.X / GRID_SZ;
  cauto GRID_Y = dst.Y / GRID_SZ;
  xorshift128_state seed {
    .a = 0x2451,
    .b = 0xBEAF,
    .c = 0xDADA,
    .d = 0x2AAF,
  };

  Vector<real> cells(GRID_X * GRID_Y);
  for (rauto cell: cells) {
    cell = ((xorshift128(seed) + SPEED) % 1'000) / 1'000.f;
    // подъём и спад яркости
    if (cell > 0.5f)
      cell = 0.5f - (cell - 0.5f);
    cell *= 2.f;
  }

  #pragma omp parallel for simd collapse(2)
  cfor (y, GRID_Y)
  cfor (x, GRID_X) {
    cfor (gy, GRID_SZ)
    cfor (gx, GRID_SZ)
      dst.set(x * GRID_SZ + gx, y * GRID_SZ + gy, Pal8::from_real(cells[y * GRID_X + x], true));
  }
}

void bgp_fading_grid_red_small(Image& dst, const int bg_state) {
  cauto SPEED = bg_state * 12;
  xorshift128_state seed {
    .a = 0x2451,
    .b = 0xBEAF,
    .c = 0xDADA,
    .d = 0x2AAF,
  };

  Vector<real> cells(dst.size);
  for (rauto cell: cells) {
    cell = ((xorshift128(seed) + SPEED) % 1'000) / 1'000.f;
    // подъём и спад яркости
    if (cell > 0.5f)
      cell = 0.5f - (cell - 0.5f);
    cell *= 2.f;
  }

  #pragma omp parallel for simd
  cfor (i, dst.size)
    dst[i] = Pal8::from_real(cells[i], true);
}

void bgp_striped_spheres(Image& dst, const int bg_state) {
  cauto SPEEd = (bg_state + 600) / 2;
  dst.fill({});

  xorshift128_state seed {
    .a = 0x2451,
    .b = 0xBEAF,
    .c = 0xDADA,
    .d = 0x2AAF,
  };

  // круги
  cfor (_, 18) {
    const Vec pos (
      xorshift128(seed) % dst.X,
      xorshift128(seed) % dst.Y );
    const real r = ((xorshift128(seed) % 1000) / 1000.f) * 100.f;
    draw_circle_filled(dst, pos, r, Pal8::red);
  }

  // инвертирующая полоска
  cfor (y, dst.Y)
  cfor (x, dst.X) {
    Pal8 color;
    if (y >= dst.Y / 2)
      color = ((x + SPEEd) % 160) > 80 ? Pal8::red : Pal8::black;
    else
      color = (std::abs(x - SPEEd) % 160) > 80 ? Pal8::red : Pal8::black;
    rauto pix = dst(x, y);
    pix = blend_diff(color, pix);
  }
}

void bgp_rotating_moire(Image& dst, const int bg_state) {
  constexpr uint LINES = 90;
  cauto center = center_point(dst);
  cauto center_2 = center + Vec(4, 9);
  cauto LINE_LEN = dst.Y - 20;
  cauto SPEED = bg_state / 200.f;
  cauto BG = Pal8::white;
  cauto FG = Pal8::black;

  dst.fill(BG);

  cfor (i, LINES) {
    cauto rad = deg_to_rad((360.f / LINES) * i);
    Vec p1(std::cos(rad), std::sin(rad));
    Vec p2(std::cos(rad + SPEED), std::sin(rad + SPEED));
    p1 *= LINE_LEN / 2;
    p2 *= LINE_LEN / 2;
    draw_line(dst, center, center + p1, FG);
    draw_line(dst, center_2, center_2 + p2, FG);
  }
}

void bgp_rotating_moire_more_lines(Image& dst, const int bg_state) {
  constexpr uint LINES = 180;
  cauto center = center_point(dst);
  cauto center_2 = center + Vec(2, 5);
  cauto LINE_LEN = dst.Y - 20;
  cauto SPEED = bg_state / 700.f;
  cauto BG = Pal8::white;
  cauto FG = Pal8::black;

  dst.fill(BG);

  cfor (i, LINES) {
    cauto rad = deg_to_rad((360.f / LINES) * i);
    Vec p1(std::cos(rad),         std::sin(rad));
    Vec p2(std::cos(rad + SPEED), std::sin(rad + SPEED));
    p1 *= LINE_LEN / 2;
    p2 *= LINE_LEN / 2;
    draw_line(dst, center, center + p1, FG);
    draw_line(dst, center_2, center_2 + p2, FG);
  }
}

void bgp_rotating_moire_rotated(Image& dst, const int bg_state) {
  constexpr uint LINES = 180;
  cauto center = center_point(dst);
  cauto LINE_LEN = dst.Y - 20;
  cauto SPEED = bg_state / 1'000.f;
  cauto SPEED_2 = bg_state / 300.f;
  cauto BG = Pal8::black;
  cauto FG = Pal8::red;

  dst.fill(BG);

  auto center_2 = center + Vec(1, 3);
  auto center_3 = center - Vec(1, 3);
  center_2 = rotate_rad(center, center_2, SPEED_2);
  center_3 = rotate_rad(center, center_3, SPEED_2);

  cfor (i, LINES) {
    cauto rad = deg_to_rad((360.f / LINES) * i);
    Vec p1(std::cos(rad - SPEED), std::sin(rad - SPEED));
    Vec p2(std::cos(rad + SPEED), std::sin(rad + SPEED));
    p1 *= LINE_LEN / 2;
    p2 *= LINE_LEN / 2;
    draw_line(dst, center_3, center_3 + p1, FG);
    draw_line(dst, center_2, center_2 + p2, FG);
  }
}

void bgp_rotating_moire_triple(Image& dst, const int bg_state) {
  constexpr uint LINES = 200;
  cauto center = center_point(dst);
  cauto center_2 = center + Vec(6, 12);
  cauto center_3 = center + Vec(-6, 12);
  cauto LINE_LEN = dst.X * 1.5;
  cauto SPEED = bg_state / 3'000.f;
  cauto BG = Pal8::white;
  cauto FG = Pal8::black;

  dst.fill(BG);

  cfor (i, LINES) {
    cauto rad = deg_to_rad((360.f / LINES) * i);
    Vec p1(std::cos(rad), std::sin(rad));
    Vec p2(std::cos(rad + SPEED), std::sin(rad + SPEED));
    Vec p3(std::cos(rad - SPEED/2), std::sin(rad - SPEED/2));
    p1 *= LINE_LEN / 2;
    p2 *= LINE_LEN / 2;
    p3 *= LINE_LEN / 2;
    draw_line(dst, center, center + p1, FG);
    draw_line(dst, center_2, center_2 + p2, FG);
    draw_line(dst, center_3, center_3 + p3, FG);
  }
}

void bgp_moire_lines(Image& dst, const int bg_state) {
  const int LINES = dst.Y / 6;
  cauto SPEED = bg_state / 450.f;
  constexpr Pal8 FG = Pal8::black;
  constexpr Pal8 BG = Pal8::white;
  cauto CENTER = center_point(dst);

  dst.fill(BG);
  cfor (i, LINES) {
    cauto Y = (dst.Y / scast<real>(LINES)) * i;
    Vec p1(dst.X - (dst.X * 1.5), Y);
    Vec p2(dst.X * 1.5, Y);
    draw_line(dst, p1, p2, FG);
    p1 = rotate_rad(CENTER, p1, SPEED);
    p2 = rotate_rad(CENTER, p2, SPEED);
    draw_line(dst, p1, p2, FG);
    p1 = rotate_rad(CENTER, p1, -2.f * SPEED);
    p2 = rotate_rad(CENTER, p2, -2.f * SPEED);
    draw_line(dst, p1, p2, FG);
  }
}

void bgp_perlin_noise(Image& dst, const int bg_state) {
  const Vec OFFSET(bg_state / 3.f, bg_state / 4.f);
  noise_2d(dst, OFFSET, 5, 3, 3, 0.333, 1);
}

void bgp_3d_sky(Image& dst, const int bg_state) {
  const Vec OFFSET(bg_state / 6.f, -bg_state / 5.f);
  
  // слой 1
  noise_2d(dst, OFFSET, 5, 3, 3, 0.1, 1);

  // слой 2
  Image layer(dst.X, dst.Y);
  noise_2d(layer, OFFSET, 5, 2, 4, 0.3, 2);
  insert_fast<&blend_max>(dst, layer);
  
  apply_brightness(dst, -70);
}

class Cellular_simul final {
public:
  struct Cell;
  using move_pf = std::function<void (Cell& self, Cellular_simul& world)>;

  struct Cell {
    bool active {};
    bool completed_step {};
    Pal8 color {};
    struct Flags {
      bool attack : 1 {};
      bool attack_self : 1 {};
      bool clone : 1 {};
      bool clone_more : 1 {};
      bool mutate : 1 {};
      bool mutate_more : 1 {};
      bool move_hv : 1 {}; // horizontal + vertical
      bool move_diagonal : 1 {};
    };
    Flags flags {};

    inline Cell* find_neighbor(int x, int y, Cellular_simul& world) {
      if (auto cell = world.get_cell(x-1, y-1); cell && cell->active) return cell;
      if (auto cell = world.get_cell(x+0, y-1); cell && cell->active) return cell;
      if (auto cell = world.get_cell(x+1, y-1); cell && cell->active) return cell;

      if (auto cell = world.get_cell(x-1, y+0); cell && cell->active) return cell;
      //if (auto cell = world.get_cell(x+0, y+0); cell && cell->active) return cell; self
      if (auto cell = world.get_cell(x+1, y+0); cell && cell->active) return cell;

      if (auto cell = world.get_cell(x-1, y+1); cell && cell->active) return cell;
      if (auto cell = world.get_cell(x+0, y+1); cell && cell->active) return cell;
      if (auto cell = world.get_cell(x+1, y+1); cell && cell->active) return cell;
      return nullptr;
    }

    inline void update(int x, int y, Cellular_simul& world) {
      if (flags.attack) {
        auto neighbor = find_neighbor(x, y, world);
        if (neighbor && !neighbor->flags.attack)
          neighbor->active = false;
      }

      if (flags.attack_self) {
        auto neighbor = find_neighbor(x, y, world);
        if (neighbor)
          neighbor->active = false;
      }

      if (flags.move_hv) {
        int dir_x = 0;
        int dir_y = 0;
        if (rndu_fast() % 2)
          dir_x = rnd_fast() % 2 ? -1 : 1;
        else
          dir_y = rnd_fast() % 2 ? -1 : 1;
        if (world.check_empty(x + dir_x, y + dir_y)) {
          Cell copy = world.get_cell_fast(x, y);
          world.set_cell_fast(x + dir_x, y + dir_y, copy);
          world.set_cell_fast(x, y, {});
        }
      }

      if (flags.move_diagonal) {
        int dir_x = rnd_fast() % 2 ? -1 : 1;
        int dir_y = rnd_fast() % 2 ? -1 : 1;
        if (world.check_empty(x + dir_x, y + dir_y)) {
          Cell copy = world.get_cell_fast(x, y);
          world.set_cell_fast(x + dir_x, y + dir_y, copy);
          world.set_cell_fast(x, y, {});
        }
      }

      if ((flags.clone && rndr_fast() <= 0.02) || (flags.clone_more && rndr_fast() <= 0.05)) {
        int dir_x = rnd_fast(-1, 1);
        int dir_y = rnd_fast(-1, 1);
        if (world.check_empty(x + dir_x, y + dir_y)) {
          Cell copy = world.get_cell_fast(x, y);
          if ((flags.mutate && rndr_fast() <= 0.2) || (flags.mutate_more && rndr_fast() <= 0.5))
            mutate(copy);
          world.set_cell_fast(x + dir_x, y + dir_y, copy);
        }
      }
    }

    inline static void mutate(Cell& cell) {
      switch (rndu_fast() % 8) {
        case 0: cell.flags.attack = !cell.flags.attack; break;
        case 1: cell.flags.attack_self = !cell.flags.attack_self; break;
        case 2: cell.flags.clone = !cell.flags.clone; break;
        case 3: cell.flags.clone_more = !cell.flags.clone_more; break;
        case 4: cell.flags.mutate = !cell.flags.mutate; break;
        case 5: cell.flags.mutate_more = !cell.flags.mutate_more; break;
        case 6: cell.flags.move_hv = !cell.flags.move_hv; break;
        case 7: cell.flags.move_diagonal = !cell.flags.move_diagonal; break;
      }
      cell.color = Pal8::from_real(cell.color.to_real() + rndr_fast(-0.1, 0.1), cell.color.is_red());
    }
  }; // Cell

  inline explicit Cellular_simul(int mx, int my, int min_cells, int max_cells)
  : _mx {mx}
  , _my {my}
  , _min_cells {min_cells}
  , _max_cells {max_cells}
  {
    assert(_mx > 0);
    assert(_my > 0);
    assert(_max_cells > 0);
    assert(_max_cells > _min_cells);
    reset();
  }

  inline void update() {
    cfor (y, _my)
    cfor (x, _mx) {
      rauto cell = get_cell_fast(x, y);
      if (cell.active && cell.completed_step == false) {
        cell.update(x, y, *this);
        cell.completed_step = true;
      }
    }

    for (rauto cell: _cells)
      cell.completed_step = false;
  }

  inline void draw(Image& dst) const {
    assert(_mx <= dst.X);
    assert(_my <= dst.Y);
    dst.fill(COLOR_BG);

    cfor (y, _my)
    cfor (x, _mx) {
      crauto cell = cget_cell_fast(x, y);
      if (cell.active)
        dst(x, y) = cell.color;
    }
  }

  inline void reset() {
    _cells.clear();
    _cells.resize(_mx * _my);
    std::fill(_cells.begin(), _cells.end(), Cell{});

    const uint CELLS = _min_cells + rndu_fast(_max_cells-1);
    cfor (_, CELLS) {
      const int x = rndu_fast(_mx-1);
      const int y = rndu_fast(_my-1);
      Cell& cell = get_cell_fast(x, y);

      const bool IS_RED = rndb_fast() % 2;
      const real COLOR_LUMA = rndr_fast(0.5, 1);
      cell.color = Pal8::from_real(COLOR_LUMA, IS_RED);

      cell.active = true;
      cell.completed_step = false;
      cell.flags.attack = rndu_fast() % 2;
      cell.flags.attack_self = rndu_fast() % 2;
      cell.flags.clone = rndu_fast() % 2;
      cell.flags.clone_more = rndu_fast() % 2;
      cell.flags.move_diagonal = rndu_fast() % 2;
      cell.flags.move_hv = rndu_fast() % 2;
      cell.flags.mutate = rndu_fast() % 2;
      cell.flags.mutate_more = rndu_fast() % 2;
    }
  }

  inline Cell* get_cell(int x, int y) {
    if (x >= 0 && x < _mx && y >= 0 && y < _my)
      return &_cells[y * _mx + x];
    return nullptr;
  }

  inline cp<Cell> cget_cell(int x, int y) const {
    if (x >= 0 && x < _mx && y >= 0 && y < _my)
      return &_cells[y * _mx + x];
    return nullptr;
  }

  inline bool check_empty(int x, int y) const {
    cauto cell = cget_cell(x, y);
    return cell && !cell->active;
  }
  
  inline void set_cell_fast(int x, int y, cr<Cell> cell) { get_cell_fast(x, y) = cell; }
  inline Cell& get_cell_fast(int x, int y) { return _cells[y * _mx + x]; }
  inline cr<Cell> cget_cell_fast(int x, int y) const { return _cells[y * _mx + x]; }
  inline Vector<Cell>& get_cells() { return _cells; }

private:
  nocopy(Cellular_simul);
  constexpr sconst Pal8 COLOR_BG = Pal8::black;
  int _mx {}, _my {}; // размеры клеточного мира
  int _min_cells {}, _max_cells {};
  Vector<Cell> _cells {};
}; // Cellular_simul

void bgp_rand_cellular_simul_x4(Image& dst, const int bg_state) {
  constexpr int SCALE = 4;
  static Cellular_simul simul(dst.X/SCALE, dst.Y/SCALE, 1'000, 5'000);
  Image buffer(dst.X/SCALE, dst.Y/SCALE);
  simul.update();
  simul.draw(buffer);
  assert(SCALE == 4);
  zoom_x4(buffer);
  insert_fast(dst, buffer);

  if (bg_state % 2'000 == 0)
    simul.reset();
}

void bgp_rand_cellular_simul_x2(Image& dst, const int bg_state) {
  constexpr int SCALE = 2;
  static Cellular_simul simul(dst.X/SCALE, dst.Y/SCALE, 10'000, 50'000);
  Image buffer(dst.X/SCALE, dst.Y/SCALE);
  simul.update();
  simul.draw(buffer);
  assert(SCALE == 2);
  zoom_x2(buffer);
  insert_fast(dst, buffer);

  if (bg_state % 2'000 == 0)
    simul.reset();
}

void bgp_rand_cellular_simul(Image& dst, const int bg_state) {
  static Cellular_simul simul(dst.X, dst.Y, 25'000, 150'000);
  simul.update();
  simul.draw(dst);

  if (bg_state % 2'000 == 0)
    simul.reset();
}
