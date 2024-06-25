#pragma once

class Image;

// https://www.nicovideo.jp/watch/sm7047818
void bgp_warabimochi(Image& dst, const int bg_state);
// сплайн-линии
void bgp_spline(Image& dst, const int bg_state);
// круг скрывающий текст https://pin.it/3qf4zkkcd
void bgp_circle_with_text(Image& dst, const int bg_state);
// чб-фон под дизером
void bgp_dither_wave(Image& dst, const int bg_state);
// узор отрисованный тайлами
void bgp_tiles_1(Image& dst, const int bg_state);
// узор отрисованный тайлами
void bgp_tiles_2(Image& dst, const int bg_state);
