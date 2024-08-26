#pragma once

class Image;

// битовой узор 1
void bgp_bit_1(Image& dst, const int bg_state);
// битовой узор 2
void bgp_bit_2(Image& dst, const int bg_state);
// битовой узор 3
void bgp_bit_3(Image& dst, const int bg_state);
// узор из пинтереста
void bgp_pinterest_1(Image& dst, const int bg_state);
// волны от случайных капель
void bgp_rain_waves(Image& dst, const int bg_state);
// волны из случайных линий
void bgp_line_waves(Image& dst, const int bg_state);
// белые полоски на красном фоне
void bgp_rotated_lines(Image& dst, const int bg_state);
// рандомные полосы
void bgp_random_lines_1(Image& dst, const int bg_state);
// рандомные полосы (меньше)
void bgp_random_lines_2(Image& dst, const int bg_state);
// узор из меандров на чёрном фоне
void bgp_labyrinth_1(Image& dst, const int bg_state);
// случайный узор из углов на чёрном фоне
void bgp_labyrinth_2(Image& dst, const int bg_state);
// 3D куб из точек
void bgp_3d_atomar_cube(Image& dst, const int bg_state);
// 3D ландшафт
void bgp_3d_terrain(Image& dst, const int bg_state);
// 3D ландшафт (грубая версия)
void bgp_3d_terrain_ps1(Image& dst, const int bg_state);
// 3D звёздочки на земле
void bgp_3d_flat_stars(Image& dst, const int bg_state);
// 3D волны
void bgp_3d_waves(Image& dst, const int bg_state);
// надписи перекатываются вверх и растягиваются
void bgp_hpw_text_lines(Image& dst, const int bg_state);
// 3D волны дождя
void bgp_3d_rain_waves(Image& dst, const int bg_state);
// множество статичных окружностей из центра
void bgp_circles(Image& dst, const int bg_state);
// как bgp_circles но с учётом предыдущего фона
void bgp_circles_2(Image& dst, const int bg_state);
// демосценерский эффект муара на окружностях
void bgp_circles_moire(Image& dst, const int bg_state);
// демосценерский эффект муара на окружностях (вторая вариация)
void bgp_circles_moire_2(Image& dst, const int bg_state);
// порт моего шейдера с shader sandbox (с чёрными полосами)
void bgp_red_circles_1(Image& dst, const int bg_state);
// порт моего шейдера с shader sandbox
void bgp_red_circles_2(Image& dst, const int bg_state);
// узор с пинтереста - пиксельные шрифты
void bgp_pixel_font(Image& dst, const int bg_state);
// узор с пинтереста - дробные цифры
void bgp_numbers(Image& dst, const int bg_state);
// узор с пинтереста - дробные цифры c разными оттенками
void bgp_numbers_alpha(Image& dst, const int bg_state);
// рандомные айпишники
void bgp_ipv4(Image& dst, const int bg_state);
// рандомные айпишники xxx.xxx.xxx.xxx
void bgp_ipv4_2(Image& dst, const int bg_state);
// рандомные символы
void bgp_unicode(Image& dst, const int bg_state);
// рандомные символы на красном фоне
void bgp_unicode_red(Image& dst, const int bg_state);
// порт glsl шейдера с светящимися сферами (https://glslsandbox.com/e#109700.0)
void bgp_glsl_spheres(Image& dst, const int bg_state);
// часы с циферблатом на 12 цифр
void bgp_clock(Image& dst, const int bg_state);
// часы с циферблатом на 24 цифр
void bgp_clock_24(Image& dst, const int bg_state);
// рандомный график
void bgp_graph(Image& dst, const int bg_state);
