#pragma once
#include "util/file/file.hpp"

inline Bytes table_inv {};
inline Bytes table_inv_safe {};
inline Bytes table_inc_safe {};
inline Bytes table_dec_safe {};
inline Bytes table_add {};
inline Bytes table_add_safe {};
inline Bytes table_sub {};
inline Bytes table_sub_safe {};
inline Bytes table_and {};
inline Bytes table_and_safe {};
inline Bytes table_or {};
inline Bytes table_or_safe {};
inline Bytes table_mul {};
inline Bytes table_mul_safe {};
inline Bytes table_xor {};
inline Bytes table_xor_safe {};
inline Bytes table_diff {};
inline Bytes table_avr {};
inline Bytes table_avr_max {};
inline Bytes table_blend158 {};
inline Bytes table_max {};
inline Bytes table_min {};
inline Bytes table_overlay {};
inline Bytes table_softlight {};
inline Bytes table_fade_in_max {};
inline Bytes table_fade_out_max {};
inline Bytes table_blend_alpha {};

void load_color_tables();
void check_color_tables();
