#include <cassert>
#include "color-table.hpp"
#include "util/file/archive.hpp"
#include "game/util/game-archive.hpp"

void load_color_tables() {
  const Str dir = "resource/color table/";
  table_inv          = hpw::archive->get_file(dir + "table_inv.dat").data;
  table_inv_safe     = hpw::archive->get_file(dir + "table_inv_safe.dat").data;
  table_inc_safe     = hpw::archive->get_file(dir + "table_inc_safe.dat").data;
  table_dec_safe     = hpw::archive->get_file(dir + "table_dec_safe.dat").data;
  table_add          = hpw::archive->get_file(dir + "table_add.dat").data;
  table_add_safe     = hpw::archive->get_file(dir + "table_add_safe.dat").data;
  table_sub          = hpw::archive->get_file(dir + "table_sub.dat").data;
  table_sub_safe     = hpw::archive->get_file(dir + "table_sub_safe.dat").data;
  table_and          = hpw::archive->get_file(dir + "table_and.dat").data;
  table_and_safe     = hpw::archive->get_file(dir + "table_and_safe.dat").data;
  table_or           = hpw::archive->get_file(dir + "table_or.dat").data;
  table_or_safe      = hpw::archive->get_file(dir + "table_or_safe.dat").data;
  table_mul          = hpw::archive->get_file(dir + "table_mul.dat").data;
  table_mul_safe     = hpw::archive->get_file(dir + "table_mul_safe.dat").data;
  table_xor          = hpw::archive->get_file(dir + "table_xor.dat").data;
  table_xor_safe     = hpw::archive->get_file(dir + "table_xor_safe.dat").data;
  table_diff         = hpw::archive->get_file(dir + "table_diff.dat").data;
  table_avr          = hpw::archive->get_file(dir + "table_avr.dat").data;
  table_blend158     = hpw::archive->get_file(dir + "table_blend158.dat").data;
  table_max          = hpw::archive->get_file(dir + "table_max.dat").data;
  table_min          = hpw::archive->get_file(dir + "table_min.dat").data;
  table_overlay      = hpw::archive->get_file(dir + "table_overlay.dat").data;
  table_softlight    = hpw::archive->get_file(dir + "table_softlight.dat").data;
  table_fade_in_max  = hpw::archive->get_file(dir + "table_fade_in_max.dat").data;
  table_fade_out_max = hpw::archive->get_file(dir + "table_fade_out_max.dat").data;
  table_blend_alpha  = hpw::archive->get_file(dir + "table_blend_alpha.dat").data;
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
  assert( !table_blend158.empty());
  assert( !table_max.empty());
  assert( !table_min.empty());
  assert( !table_overlay.empty());
  assert( !table_softlight.empty());
  assert( !table_fade_in_max.empty());
  assert( !table_fade_out_max.empty());
  assert( !table_blend_alpha.empty());
} // check_color_tables
