#include <cassert>
#include "color-table.hpp"
#include "game/util/resource-helper.hpp"
#include "util/file/file.hpp"

void load_color_tables() {
  const Str dir = "resource/color table/";
  table_inv          = load_res(dir + "table_inv.dat").data;
  table_inv_safe     = load_res(dir + "table_inv_safe.dat").data;
  table_inc_safe     = load_res(dir + "table_inc_safe.dat").data;
  table_dec_safe     = load_res(dir + "table_dec_safe.dat").data;
  table_add          = load_res(dir + "table_add.dat").data;
  table_add_safe     = load_res(dir + "table_add_safe.dat").data;
  table_sub          = load_res(dir + "table_sub.dat").data;
  table_sub_safe     = load_res(dir + "table_sub_safe.dat").data;
  table_and          = load_res(dir + "table_and.dat").data;
  table_and_safe     = load_res(dir + "table_and_safe.dat").data;
  table_or           = load_res(dir + "table_or.dat").data;
  table_or_safe      = load_res(dir + "table_or_safe.dat").data;
  table_mul          = load_res(dir + "table_mul.dat").data;
  table_mul_safe     = load_res(dir + "table_mul_safe.dat").data;
  table_xor          = load_res(dir + "table_xor.dat").data;
  table_xor_safe     = load_res(dir + "table_xor_safe.dat").data;
  table_diff         = load_res(dir + "table_diff.dat").data;
  table_avr          = load_res(dir + "table_avr.dat").data;
  table_avr_max      = load_res(dir + "table_avr_max.dat").data;
  table_blend158     = load_res(dir + "table_blend158.dat").data;
  table_max          = load_res(dir + "table_max.dat").data;
  table_min          = load_res(dir + "table_min.dat").data;
  table_overlay      = load_res(dir + "table_overlay.dat").data;
  table_softlight    = load_res(dir + "table_softlight.dat").data;
  table_fade_in_max  = load_res(dir + "table_fade_in_max.dat").data;
  table_fade_out_max = load_res(dir + "table_fade_out_max.dat").data;
  table_blend_alpha  = load_res(dir + "table_blend_alpha.dat").data;
} // load_color_tables

void check_color_tables() {
  assert( !table_inv.empty());
  assert( !table_inv_safe.empty());
  assert( !table_inc_safe.empty());
  assert( !table_dec_safe.empty());
  assert( !table_add.empty());
  assert( !table_add_safe.empty());
  assert( !table_sub.empty());
  assert( !table_sub_safe.empty());
  assert( !table_and.empty());
  assert( !table_and_safe.empty());
  assert( !table_or.empty());
  assert( !table_or_safe.empty());
  assert( !table_mul.empty());
  assert( !table_mul_safe.empty());
  assert( !table_xor.empty());
  assert( !table_xor_safe.empty());
  assert( !table_diff.empty());
  assert( !table_avr.empty());
  assert( !table_avr_max.empty());
  assert( !table_blend158.empty());
  assert( !table_max.empty());
  assert( !table_min.empty());
  assert( !table_overlay.empty());
  assert( !table_softlight.empty());
  assert( !table_fade_in_max.empty());
  assert( !table_fade_out_max.empty());
  assert( !table_blend_alpha.empty());
} // check_color_tables
