#pragma once

class Image;

// https://www.nicovideo.jp/watch/sm7047818
void bgp_warabimochi(Image& dst, const int bg_state);
// сплайн-линии
void bgp_spline(Image& dst, const int bg_state);
// сплайн-линии увеличенные
void bgp_spline_zoom(Image& dst, const int bg_state);
// линии отскакивают от экрана
void bgp_bounding_lines(Image& dst, const int bg_state);
// круги отскакивают от экрана
void bgp_bounding_circles(Image& dst, const int bg_state);
// круги с кругами внутри отскакивают от экрана
void bgp_bounding_repeated_circles(Image& dst, const int bg_state);
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
// мерцающий фон
void bgp_epilepsy(Image& dst, const int bg_state);
// красный фон с градиентом и дизерингом
void bgp_red_gradient(Image& dst, const int bg_state);
// красный фон с градиентом и более грубым дизерингом
void bgp_red_gradient_2(Image& dst, const int bg_state);
