#pragma once

class Image;

// заблюренная жижа
void bgp_liquid(Image& dst, const int bg_state);
// заблюренная жижа (серая)
void bgp_liquid_gray(Image& dst, const int bg_state);
// сплайны с подписями кордов
void bgp_trajectory(Image& dst, const int bg_state);
// медленно сменяющая цвет плитка
void bgp_fading_grid(Image& dst, const int bg_state);
// медленно сменяющая цвет плитка (тусклая)
void bgp_fading_grid_black(Image& dst, const int bg_state);
// медленно сменяющая цвет плитка (красная)
void bgp_fading_grid_red(Image& dst, const int bg_state);
// медленно сменяющая цвет плитка (красная 1:1)
void bgp_fading_grid_red_small(Image& dst, const int bg_state);
// медленно сменяющая цвет плитка (с дизерингом)
void bgp_fading_grid_dithered(Image& dst, const int bg_state);
// ширики в зебру c XOR (https://pin.it/15XSTrGE6)
void bgp_striped_spheres(Image& dst, const int bg_state);
