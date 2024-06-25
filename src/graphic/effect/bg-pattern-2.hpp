#pragma once

class Image;

// https://www.nicovideo.jp/watch/sm7047818
void bgp_warabimochi(Image& dst, const int bg_state);
// сплайн-линии
void bgp_spline(Image& dst, const int bg_state);
// круг скрывающий текст https://pin.it/3qf4zkkcd
void bgp_circle_with_text(Image& dst, const int bg_state);
// узор под дизером с отдалением
void bgp_dither_wave(Image& dst, const int bg_state);
// узор под дизером
void bgp_dither_wave_2(Image& dst, const int bg_state);
// узор отрисованный тайлами
void bgp_tiles_1(Image& dst, const int bg_state);
// узор отрисованный тайлами
void bgp_tiles_2(Image& dst, const int bg_state);
// просто Ниссан Скайлан на фоне
void bgp_skyline(Image& dst, const int bg_state);
/* белые полоски движутся так быстро, что
останавливаются при определённой скорости */
void bgp_fast_lines(Image& dst, const int bg_state);
// красная версия bgp_fast_lines
void bgp_fast_lines_red(Image& dst, const int bg_state);
