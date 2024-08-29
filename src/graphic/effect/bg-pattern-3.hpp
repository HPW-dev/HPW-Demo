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
