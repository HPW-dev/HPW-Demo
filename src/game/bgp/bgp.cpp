#include "bgp.hpp"
#include "bgp-pack-1.hpp"
#include "bgp-pack-2.hpp"
#include "bgp-pack-3.hpp"
#include "bgp-pack-4.hpp"

[[nodiscard]] bgp_pf get_bgp(cr<Str> name) {
  /*
  sconst Rnd_table<decltype(bg_pattern_pf)> bg_patterns {{
  // Пак 1:
  #if 1
    &bgp_hpw_text_lines,
    &bgp_bit_1,
    &bgp_bit_2,
    &bgp_pinterest_1,
    &bgp_random_lines_1,
    &bgp_random_lines_2,
    &bgp_3d_atomar_cube,
    &bgp_circles,
    &bgp_circles_2,
    &bgp_circles_moire,
    &bgp_circles_moire_2,
    &bgp_red_circles_1,
    &bgp_red_circles_2,
    &bgp_pixel_font,
    &bgp_numbers,
    &bgp_numbers_alpha,
    &bgp_ipv4,
    &bgp_ipv4_2,
    &bgp_unicode,
    &bgp_unicode_red,
    &bgp_glsl_spheres,
    &bgp_clock,
    &bgp_clock_24,
    &bgp_graph,
    #ifndef ECOMEM
      &bgp_3d_rain_waves,
      &bgp_3d_waves,
      &bgp_3d_terrain,
      &bgp_3d_terrain_ps1,
      &bgp_3d_flat_stars,
      &bg_copy_1,
      &bg_copy_2,
      &bg_copy_3,
      &bg_copy_4,
      &bgp_rain_waves,
      &bgp_line_waves,
      &bgp_rotated_lines,
      &bgp_labyrinth_1,
      &bgp_labyrinth_2,
    #endif
  #endif
  // Пак 2:
  #if 1
    &bgp_self_code,
    &bgp_noise,
    &bgp_tile_corruption,
    &bgp_deep_circles,
    &bgp_deep_circles_red,
    &bgp_deep_lines,
    &bgp_deep_lines_2,
    &bgp_deep_lines_3,
    &bgp_deep_lines_red,
    &bgp_deep_lines_red_2,
    &bgp_deep_lines_red_3,
    &bgp_dither_wave,
    &bgp_dither_wave_2,
    &bgp_fast_lines,
    &bgp_fast_lines_red,
    &bgp_epilepsy,
    &bgp_red_gradient,
    &bgp_red_gradient_2,
    &bgp_spline_zoom,
    &bgp_spline,
    &bgp_bounding_lines,
    &bgp_bounding_repeated_circles,
    &bgp_repeated_rectangles,
    &bgp_bounding_circles,
    &bgp_skyline,
    &bgp_warabimochi,
    &bgp_circle_with_text,
    &bgp_red_circle_white,
    &bgp_red_circle_black,
    &bgp_tiles_1,
    &bgp_tiles_2,
    &bgp_tiles_3,
    &bgp_tiles_4,
    &bgp_zoom_dst,
  #endif
  // Пак 3:
  #if 1
    &bgp_3d_sky,
    &bgp_perlin_noise,
    &bgp_liquid,
    &bgp_liquid_gray,
    &bgp_trajectory,
    &bgp_fading_grid,
    &bgp_fading_grid_red,
    &bgp_fading_grid_red_small,
    &bgp_fading_grid_dithered,
    &bgp_striped_spheres,
    &bgp_rotating_moire,
    &bgp_rotating_moire_more_lines,
    &bgp_rotating_moire_rotated,
    &bgp_rotating_moire_triple,
    &bgp_moire_lines,
    &bgp_nano_columns,
    #ifndef ECOMEM
      &bgp_rand_cellular_simul,
      &bgp_rand_cellular_simul_x2,
      &bgp_rand_cellular_simul_x4,
    #endif
  #endif
  // Пак 4:
  #if 1
    &bgp_physics_1,
    &bgp_physics_2,
  #endif
  }}; // bg_patterns table*/
  return {}; // TODO
}

[[nodiscard]] Strs get_bgp_names() {
  return {}; // TODO
}

void randomize_menu_bgp() {
  // TODO
}
