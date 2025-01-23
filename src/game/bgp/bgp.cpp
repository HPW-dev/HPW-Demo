#include <cassert>
#include <unordered_map>
#include <algorithm>
#include "bgp.hpp"
#include "bgp-pack-1.hpp"
#include "bgp-pack-2.hpp"
#include "bgp-pack-3.hpp"
#include "bgp-pack-4.hpp"
#include "util/math/random.hpp"
#include "util/log.hpp"

// привящка имён к функциям генерации узоров
static std::unordered_map<Str, bgp_pf> _g_bgps {
#if 1 // Пак 1:
  {"hpw_text_lines", &bgp_hpw_text_lines},
  {"bit_1", &bgp_bit_1},
  {"bit_2", &bgp_bit_2},
  {"pinterest_1", &bgp_pinterest_1},
  {"random_lines_1", &bgp_random_lines_1},
  {"random_lines_2", &bgp_random_lines_2},
  {"3d_atomar_cube", &bgp_3d_atomar_cube},
  {"circles", &bgp_circles},
  {"circles_2", &bgp_circles_2},
  {"circles_moire", &bgp_circles_moire},
  {"circles_moire_2", &bgp_circles_moire_2},
  {"red_circles_1", &bgp_red_circles_1},
  {"red_circles_2", &bgp_red_circles_2},
  {"pixel_font", &bgp_pixel_font},
  {"numbers", &bgp_numbers},
  {"numbers_alpha", &bgp_numbers_alpha},
  {"ipv4", &bgp_ipv4},
  {"ipv4_2", &bgp_ipv4_2},
  {"unicode", &bgp_unicode},
  {"unicode_red", &bgp_unicode_red},
  {"glsl_spheres", &bgp_glsl_spheres},
  {"clock", &bgp_clock},
  {"clock_24", &bgp_clock_24},
  {"graph", &bgp_graph},
  #ifndef ECOMEM
    {"3d_rain_waves", &bgp_3d_rain_waves},
    {"3d_waves", &bgp_3d_waves},
    {"3d_terrain", &bgp_3d_terrain},
    {"3d_terrain_ps1", &bgp_3d_terrain_ps1},
    {"3d_flat_stars", &bgp_3d_flat_stars},
    {"copy_1", &bgp_copy_1},
    {"copy_2", &bgp_copy_2},
    {"copy_3", &bgp_copy_3},
    {"copy_4", &bgp_copy_4},
    {"rain_waves", &bgp_rain_waves},
    {"line_waves", &bgp_line_waves},
    {"rotated_lines", &bgp_rotated_lines},
    {"labyrinth_1", &bgp_labyrinth_1},
    {"labyrinth_2", &bgp_labyrinth_2},
  #endif
#endif

#if 1 // Пак 2:
  {"self_code", &bgp_self_code},
  {"noise", &bgp_noise},
  {"tile_corruption", &bgp_tile_corruption},
  {"deep_circles", &bgp_deep_circles},
  {"deep_circles_red", &bgp_deep_circles_red},
  {"deep_lines", &bgp_deep_lines},
  {"deep_lines_2", &bgp_deep_lines_2},
  {"deep_lines_3", &bgp_deep_lines_3},
  {"deep_lines_red", &bgp_deep_lines_red},
  {"deep_lines_red_2", &bgp_deep_lines_red_2},
  {"deep_lines_red_3", &bgp_deep_lines_red_3},
  {"dither_wave", &bgp_dither_wave},
  {"dither_wave_2", &bgp_dither_wave_2},
  {"fast_lines", &bgp_fast_lines},
  {"fast_lines_red", &bgp_fast_lines_red},
  {"epilepsy", &bgp_epilepsy},
  {"red_gradient", &bgp_red_gradient},
  {"red_gradient_2", &bgp_red_gradient_2},
  {"spline_zoom", &bgp_spline_zoom},
  {"spline", &bgp_spline},
  {"bounding_lines", &bgp_bounding_lines},
  {"bounding_repeated_circles", &bgp_bounding_repeated_circles},
  {"repeated_rectangles", &bgp_repeated_rectangles},
  {"bounding_circles", &bgp_bounding_circles},
  {"skyline", &bgp_skyline},
  {"warabimochi", &bgp_warabimochi},
  {"circle_with_text", &bgp_circle_with_text},
  {"red_circle_white", &bgp_red_circle_white},
  {"red_circle_black", &bgp_red_circle_black},
  {"tiles_1", &bgp_tiles_1},
  {"tiles_2", &bgp_tiles_2},
  {"tiles_3", &bgp_tiles_3},
  {"tiles_4", &bgp_tiles_4},
  {"zoom_dst", &bgp_zoom_dst},
  #ifndef ECOMEM
  // TODO ?
  #endif
#endif

#if 1 // Пак 3:
  {"3d_sky", &bgp_3d_sky},
  {"perlin_noise", &bgp_perlin_noise},
  {"liquid", &bgp_liquid},
  {"liquid_gray", &bgp_liquid_gray},
  {"trajectory", &bgp_trajectory},
  {"fading_grid", &bgp_fading_grid},
  {"fading_grid_red", &bgp_fading_grid_red},
  {"fading_grid_red_small", &bgp_fading_grid_red_small},
  {"fading_grid_dithered", &bgp_fading_grid_dithered},
  {"striped_spheres", &bgp_striped_spheres},
  {"rotating_moire", &bgp_rotating_moire},
  {"rotating_moire_more_lines", &bgp_rotating_moire_more_lines},
  {"rotating_moire_rotated", &bgp_rotating_moire_rotated},
  {"rotating_moire_triple", &bgp_rotating_moire_triple},
  {"moire_lines", &bgp_moire_lines},
  {"nano_columns", &bgp_nano_columns},
  #ifndef ECOMEM
    {"rand_cellular_simul", &bgp_rand_cellular_simul},
    {"rand_cellular_simul_x2", &bgp_rand_cellular_simul_x2},
    {"rand_cellular_simul_x4", &bgp_rand_cellular_simul_x4},
  #endif
#endif

#if 1 // Пак 4:
  {"physics_1", &bgp_physics_1},
  {"physics_2", &bgp_physics_2},
  #ifndef ECOMEM
  // TODO ?
  #endif
#endif
}; // _g_bgps

[[nodiscard]] bgp_pf get_bgp(cr<Str> name) {
  try {
    return _g_bgps.at(name);
  } catch (...) {}

  hpw_warning("не удалось найти фон с именем \"" + name + "\", будет выбран фон \"copy_1\"");
  return &bgp_copy_1;
}

[[nodiscard]] Strs get_bgp_names() {
  Strs ret;
  for (crauto [name, bpg]: _g_bgps)
    ret.push_back(name);
  return ret;
}

void randomize_menu_bgp() {
  cauto mx = _g_bgps.size();
  assert(mx > 0);
  cauto idx = rndu_fast(mx-1);
  auto it = _g_bgps.begin();
  std::advance(it, idx);
  assert(it != _g_bgps.end());
  crauto [name, bgp] = *it;
  hpw::menu_bgp = bgp;
  hpw::menu_bgp_name = name;
}
