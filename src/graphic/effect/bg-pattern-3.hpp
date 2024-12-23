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
// вращающийся муар (https://pin.it/1iugapuvu)
void bgp_rotating_moire(Image& dst, const int bg_state);
// как bgp_rotating_moire, но больше линий
void bgp_rotating_moire_more_lines(Image& dst, const int bg_state);
// как bgp_rotating_moire, но оба колеса крутятся
void bgp_rotating_moire_rotated(Image& dst, const int bg_state);
// как bgp_rotating_moire, но три колеса
void bgp_rotating_moire_triple(Image& dst, const int bg_state);
// крутятся параллельные прямые
void bgp_moire_lines(Image& dst, const int bg_state);
// шум похожий на шум Перлена
void bgp_perlin_noise(Image& dst, const int bg_state);
// многослойные облака
void bgp_3d_sky(Image& dst, const int bg_state);
// случайные клеточные симуляции
void bgp_rand_cellular_simul(Image& dst, const int bg_state);
// случайные клеточные симуляции
void bgp_rand_cellular_simul_x2(Image& dst, const int bg_state);
// случайные клеточные симуляции
void bgp_rand_cellular_simul_x4(Image& dst, const int bg_state);
// нано0столбики из узора с пинтереста
void bgp_nano_columns(Image& dst, const int bg_state);
