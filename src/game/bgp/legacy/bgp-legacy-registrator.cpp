#include <cmath>
#include <cassert>
#include "game/bgp/bgp.hpp"
#include "util/hpw-util.hpp"
#include "util/macro.hpp"
#include "bgp-pack-1.hpp"
#include "bgp-pack-2.hpp"
#include "bgp-pack-3.hpp"
#include "bgp-pack-4.hpp"

using Bgp_fn = void (*)(Image& dst, const int bg_state);

template <Bgp_fn FN> class Bgp_wrapper final: public bgp::Bgp {
  mutable Delta_time _timer {};
public:
  inline void update(Delta_time dt) override { _timer += dt; }
  inline void draw(Image& dst) const override { assert(FN), FN(dst, std::floor(pps(_timer))); }
}; // Bgp_wrapper

#define REG_BGP(FN) inline static bgp::Registrator< Bgp_wrapper<&FN> > CONCAT(_bgp_reg_for__, FN) (STRINGIFY(FN));

#if 1 // Пак 1:
  REG_BGP(bgp_hpw_text_lines)
  REG_BGP(bgp_bit_1)
  REG_BGP(bgp_bit_2)
  REG_BGP(bgp_pinterest_1)
  REG_BGP(bgp_random_lines_1)
  REG_BGP(bgp_random_lines_2)
  REG_BGP(bgp_3d_atomar_cube)
  REG_BGP(bgp_circles)
  REG_BGP(bgp_circles_2)
  REG_BGP(bgp_circles_moire)
  REG_BGP(bgp_circles_moire_2)
  REG_BGP(bgp_red_circles_1)
  REG_BGP(bgp_red_circles_2)
  REG_BGP(bgp_pixel_font)
  REG_BGP(bgp_numbers)
  REG_BGP(bgp_numbers_alpha)
  REG_BGP(bgp_ipv4)
  REG_BGP(bgp_ipv4_2)
  REG_BGP(bgp_unicode)
  REG_BGP(bgp_unicode_red)
  REG_BGP(bgp_glsl_spheres)
  REG_BGP(bgp_clock)
  REG_BGP(bgp_clock_24)
  REG_BGP(bgp_graph)
  #ifndef ECOMEM
    REG_BGP(bgp_3d_rain_waves)
    REG_BGP(bgp_3d_waves)
    REG_BGP(bgp_3d_terrain)
    REG_BGP(bgp_3d_terrain_ps1)
    REG_BGP(bgp_3d_flat_stars)
    REG_BGP(bgp_copy_1)
    REG_BGP(bgp_copy_2)
    REG_BGP(bgp_copy_3)
    REG_BGP(bgp_copy_4)
    REG_BGP(bgp_rain_waves)
    REG_BGP(bgp_line_waves)
    REG_BGP(bgp_rotated_lines)
    REG_BGP(bgp_labyrinth_1)
    REG_BGP(bgp_labyrinth_2)
  #endif
#endif

#if 1 // Пак 2:
  REG_BGP(bgp_self_code)
  REG_BGP(bgp_noise)
  REG_BGP(bgp_tile_corruption)
  REG_BGP(bgp_deep_circles)
  REG_BGP(bgp_deep_circles_red)
  REG_BGP(bgp_deep_lines)
  REG_BGP(bgp_deep_lines_2)
  REG_BGP(bgp_deep_lines_3)
  REG_BGP(bgp_deep_lines_red)
  REG_BGP(bgp_deep_lines_red_2)
  REG_BGP(bgp_deep_lines_red_3)
  REG_BGP(bgp_dither_wave)
  REG_BGP(bgp_dither_wave_2)
  REG_BGP(bgp_fast_lines)
  REG_BGP(bgp_fast_lines_red)
  REG_BGP(bgp_epilepsy)
  REG_BGP(bgp_red_gradient)
  REG_BGP(bgp_red_gradient_2)
  REG_BGP(bgp_spline_zoom)
  REG_BGP(bgp_spline)
  REG_BGP(bgp_bounding_lines)
  REG_BGP(bgp_bounding_repeated_circles)
  REG_BGP(bgp_repeated_rectangles)
  REG_BGP(bgp_bounding_circles)
  REG_BGP(bgp_skyline)
  REG_BGP(bgp_warabimochi)
  REG_BGP(bgp_circle_with_text)
  REG_BGP(bgp_red_circle_white)
  REG_BGP(bgp_red_circle_black)
  REG_BGP(bgp_tiles_1)
  REG_BGP(bgp_tiles_2)
  REG_BGP(bgp_tiles_3)
  REG_BGP(bgp_tiles_4)
  REG_BGP(bgp_zoom_dst)
#endif

#if 1 // Пак 3:
  REG_BGP(bgp_3d_sky)
  REG_BGP(bgp_perlin_noise)
  REG_BGP(bgp_liquid)
  REG_BGP(bgp_liquid_gray)
  REG_BGP(bgp_trajectory)
  REG_BGP(bgp_fading_grid)
  REG_BGP(bgp_fading_grid_red)
  REG_BGP(bgp_fading_grid_red_small)
  REG_BGP(bgp_fading_grid_dithered)
  REG_BGP(bgp_striped_spheres)
  REG_BGP(bgp_rotating_moire)
  REG_BGP(bgp_rotating_moire_more_lines)
  REG_BGP(bgp_rotating_moire_rotated)
  REG_BGP(bgp_rotating_moire_triple)
  REG_BGP(bgp_moire_lines)
  REG_BGP(bgp_nano_columns)
  #ifndef ECOMEM
    REG_BGP(bgp_rand_cellular_simul)
    REG_BGP(bgp_rand_cellular_simul_x2)
    REG_BGP(bgp_rand_cellular_simul_x4)
  #endif
#endif

#if 1 // Пак 4:
  REG_BGP(bgp_physics_1)
  REG_BGP(bgp_physics_2)
#endif

#undef REG_BGP
