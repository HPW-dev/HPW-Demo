#include <omp.h>
#include <array>
#include <algorithm>
#include <cassert>
#include "bg-pattern-2.hpp"
#include "bg-pattern.hpp"
#include "graphic/image/image.hpp"
#include "graphic/effect/dither.hpp"
#include "graphic/util/blur.hpp"
#include "graphic/util/graphic-util.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/util/resize.hpp"
#include "game/core/fonts.hpp"
#include "game/core/sprites.hpp"
#include "util/math/random.hpp"

// делает случайные числа
uint prng(uint& state) {
  state %= 37'891u;
  state *= 123'456'789u;
  state += 313'242u;
  return state;
}

void bgp_warabimochi(Image& dst, const int bg_state) {
  // фон с тачкой
  cauto warabimochi = hpw::store_sprite->find("resource/image/loading logo/warabimochi.png");
  assert(warabimochi);
  insert_fast(dst, warabimochi->image());
  // размыть и затенить фон
  #ifdef DEBUG
    blur_fast(dst, 7);
  #else
    adaptive_blur(dst, 5);
  #endif

  // комменты
  static const Vector<utf32> comments {
    U"​​каеф",
    U"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW",
    U"VVVVVVVVVVVVVVVVVVVVVVVVVVVVV",
    U"-------------------!!",
    U"~~~~~~~~~~~~~!!!",
    U"(^_^)",
    U"(゜▼゜＊）ウヒヒヒ",
    U"(゜∇ ゜)ブヒャヒャヒャヒャ",
    U"( ͡° ͜ʖ ͡°)",
    U"( ͡°( ͡° ͜ʖ( ͡° ͜ʖ ͡°)ʖ ͡°) ͡°)",
    U"(づ｡◕‿‿◕｡)づ",
    U"♥",
    U"♥ ♥ ♥",
    U"♥♥♥♥ ♥ ♥♥♥♥♥ ♥ ♥♥♥♥♥ ♥ ♥♥♥♥♥ ♥ ♥",
    U"(´・ω・)っ由",
    U"| (• ◡•)|",
    U"(͠≖ ͜ʖ͠≖)",
    U"ヽ(｀Д´)ﾉ",
    U"ԅ(≖‿≖ԅ)",
    U"⊂(◉‿◉)つ",
    U"ｷﾀ━━(ﾟ∀ﾟ)━━ｯ!!",
    U"WARABIMOCHI",
    U"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
    U"A A A A A A A A A A A A A A A A A A A A",
    U"HPW",
    U"H.P.W",
    U"UwU",
    U"OwO",
    U"Bruh...",
    U"Клубничный выходной",
    U"HHHHHHHHHHHPPPPPPPPPPPPPPPWWWWWWWWWWW!!!!!!!",
    U"Чё за грузовик на фоне?",
    U"Когда релиз?",
    U"Что значит H.P.W???",
    U"Uhhh...",
    U"SUS",
    U"LEL",
    U"KEK",
    U"LOL",
    U"lololololololololol",
    U"lolololololololololololololololololololololol",
    U"get rekt",
    U"Ke ke kek eke",
    U"Ke ke ke",
    U"ㅋㅋㅋ",
    U"ㅋㅋㅋㅋㅋㅋㅋㅋㅋㅋㅋㅋㅋㅋㅋㅋㅋㅋㅋㅋㅋ",
    U"哈哈哈",
    U"哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈",
    U"けけけ",
    U"けけけけけけけけけけけけけけけ",
    U")",
    U")))",
    U")))))))))))))))))))))))))))))))))))))))))))))",
    U"###############################",
    U"@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@",
    U"███",
    U"░░░░",
    U"╔═════════════════════╗\n"
    U"║ К л у б н и ч н ы й ║\n"
    U"║   в ы х о д н о й   ║\n"
    U"╚═════════════════════╝",
    U"лол",
    U"sample text",
    U"здесь могла быть ваша реклама",
    U"*105#",
    U"https://github.com/HPW-dev/HPW-Demo",
    U"https://gitflic.ru/project/hpw-dev/hpw-demo",
  }; // comments

  // слой комментов 
  Image comments_layer(dst.X, dst.Y, Pal8::black);
  cfor (i, 500) {
    const real offset_x = (i * 545'234 + 341'234) % 124'245;
    const real offset_y = (i * 4'123'456 - 3'789'013 + i * 4) % 643'235;
    const real speed = 0.5f + std::fmod((i * 1'526'234 + 4'124'123 + i) % 436'235, 7.f);
    const real pos_x = std::fmod((offset_x - (bg_state * speed)), 1'500.f) - 500.f;
    const real pos_y = std::fmod(offset_y, dst.Y);
    const Vec pos(pos_x, pos_y);
    cnauto comment = comments.at(i % comments.size());
    graphic::font->draw(comments_layer, pos, comment);
  }

  // покрасить слой комментов в красный и отобразить на фоне
  to_red(comments_layer);
  insert<&blend_no_black>(dst, comments_layer);
} // bgp_warabimochi

void bgp_spline_zoom(Image& dst, const int bg_state) {
  const real state = bg_state * 0.04f;
  Image buffer(dst.X/4, dst.Y/4);

  // случайные точки на экране
  uint seed = 97997; // ALMONDS
  constexpr uint points_sz = 25;
  std::array<Vec, points_sz> points;
  for (nauto point: points) {
    const Vec vel (
      std::fmod(prng(seed), 8.f) - 4.f,
      std::fmod(prng(seed), 8.f) - 4.f
    );
    point = Vec (
      std::fmod( std::abs(std::fmod(prng(seed), buffer.X) + vel.x * state), buffer.X),
      std::fmod( std::abs(std::fmod(prng(seed), buffer.Y) + vel.y * state), buffer.Y)
    );
  }

  buffer.fill(Pal8::black);

  static_assert(points_sz > 4);
  for (uint i = 3; i < points_sz; ++i)
    draw_spline(buffer, points[i-3], points[i-2], points[i-1], points[i], Pal8::red);
  
  zoom_x4(buffer);
  insert_fast(dst, buffer);
} // bgp_spline_zoom

void bgp_spline(Image& dst, const int bg_state) {
  const real state = bg_state * 0.04f;

  // случайные точки на экране
  uint seed = 97997; // ALMONDS
  constexpr uint points_sz = 75;
  std::array<Vec, points_sz> points;
  for (nauto point: points) {
    const Vec vel (
      std::fmod(prng(seed), 8.f) - 4.f,
      std::fmod(prng(seed), 8.f) - 4.f
    );
    point = Vec (
      std::fmod( std::abs(std::fmod(prng(seed), dst.X) + vel.x * state), dst.X),
      std::fmod( std::abs(std::fmod(prng(seed), dst.Y) + vel.y * state), dst.Y)
    );
  }

  dst.fill(Pal8::black);

  static_assert(points_sz > 4);
  for (uint i = 3; i < points_sz; ++i)
    draw_spline(dst, points[i-3], points[i-2], points[i-1], points[i], Pal8::white);
} // bgp_spline

void bgp_bounding_repeated_circles(Image& dst, const int bg_state) {
  const real state = bg_state * 0.066666f;

  // случайные точки на экране
  uint seed = 97997; // ALMONDS
  constexpr uint points_sz = 8;
  std::array<Vec, points_sz> points;
  for (nauto point: points) {
    const Vec vel (
      std::fmod(prng(seed), 8.f) - 4.f,
      std::fmod(prng(seed), 8.f) - 4.f
    );
    point = Vec (
      std::fmod( std::abs(std::fmod(prng(seed), dst.X) + vel.x * state), dst.X),
      std::fmod( std::abs(std::fmod(prng(seed), dst.Y) + vel.y * state), dst.Y)
    );
  }

  dst.fill(Pal8::black);

  static_assert(points_sz > 2);
  for (uint i = 1; i < points_sz; ++i) {
    auto r = distance(points[i], points[i-1]);
    cauto color_mul = 1.f / r;
    while (r > 0) {
      draw_circle<&blend_max>( dst, points[i], r,
        Pal8::from_real(1.f - (r * color_mul), true)
      );
      r -= 5.f;
    }
  }
} // bgp_bounding_repeated_circles

void bgp_bounding_circles(Image& dst, const int bg_state) {
  const real state = bg_state * 0.1414f;

  // случайные точки на экране
  uint seed = 97997; // ALMONDS
  constexpr uint points_sz = 9;
  std::array<Vec, points_sz> points;
  for (nauto point: points) {
    const Vec vel (
      std::fmod(prng(seed), 8.f) - 4.f,
      std::fmod(prng(seed), 8.f) - 4.f
    );
    point = Vec (
      std::fmod( std::abs(std::fmod(prng(seed), dst.X) + vel.x * state), dst.X),
      std::fmod( std::abs(std::fmod(prng(seed), dst.Y) + vel.y * state), dst.Y)
    );
  }

  dst.fill(Pal8::black);

  static_assert(points_sz > 2);
  for (uint i = 1; i < points_sz; ++i) {
    draw_circle(dst, points[i], distance(points[i], points[i-1]), Pal8::white);
  }
} // bgp_bounding_circles

void bgp_bounding_lines(Image& dst, const int bg_state) {
  const real state = bg_state * 0.25f;

  // случайные точки на экране
  uint seed = 97997; // ALMONDS
  constexpr uint points_sz = 35;
  std::array<Vec, points_sz> points;
  for (nauto point: points) {
    const Vec vel (
      std::fmod(prng(seed), 8.f) - 4.f,
      std::fmod(prng(seed), 8.f) - 4.f
    );
    point = Vec (
      std::fmod( std::abs(std::fmod(prng(seed), dst.X) + vel.x * state), dst.X),
      std::fmod( std::abs(std::fmod(prng(seed), dst.Y) + vel.y * state), dst.Y)
    );
  }

  dst.fill(Pal8::black);

  static_assert(points_sz > 2);
  for (uint i = 1; i < points_sz; ++i)
    draw_line(dst, points[i-1], points[i], Pal8::white);
} // bgp_bounding_lines

void bgp_dither_wave(Image& dst, const int bg_state) {
  const real state = bg_state * 3.2f;

  #pragma omp parallel for simd collapse(2)
  cfor (y, dst.Y)
  cfor (x, dst.X) {
    const real zoom = 0.07f - std::cos(state * 0.0005f) * 0.07f;
    real color = std::cos((x + state) * zoom + y * zoom);
    color *= std::tan((-x + state) * zoom + y * zoom);
    color *= 0.2f;
    dst(x, y) = std::fmod(color * 255.f, 255.f);
  }

  dither_bayer16x16_1bit(dst);
}

void bgp_dither_wave_2(Image& dst, const int bg_state) {
  const real state = (bg_state % 2'000) * 0.0005f;
  const real mul_x = 1.0 / dst.X;

  #pragma omp parallel for simd collapse(2)
  cfor (y, dst.Y)
  cfor (x, dst.X) {
    const Pal8 color = std::fmod((y * x) + std::cos(state) * 100'000.0f, 40.f) > 20.f
      ? Pal8::from_real(x * mul_x) : Pal8{};
    dst(x, y) = color;
  }

  dither_bayer16x16_1bit(dst);
}

void bgp_fast_lines(Image& dst, const int bg_state) {
  const real state = std::fmod(bg_state * 0.0005f, 1'000.f);
  const real mul_x = 1.0 / dst.X;

  #pragma omp parallel for simd collapse(2)
  cfor (y, dst.Y)
  cfor (x, dst.X) {
    const Pal8 color = std::fmod(y + std::cos(state) * 100'000.0f, 40.f) > 20.f
      ? Pal8::from_real(x * mul_x) : Pal8{};
    dst(x, y) = blend_avr(color, dst(x, y));
  }
}

void bgp_fast_lines_red(Image& dst, const int bg_state) {
  const real state = std::fmod(bg_state * 0.0005f, 1'000.f);
  const real mul_x = 1.0 / dst.X;

  #pragma omp parallel for simd collapse(2)
  cfor (y, dst.Y)
  cfor (x, dst.X) {
    const Pal8 color = std::fmod(y + x + std::cos(state) * 100'000.0f, 40.f) > 20.f
      ? Pal8::from_real(x * mul_x, true) : Pal8{};
    dst(x, y) = color;
  }
}

void bgp_skyline(Image& dst, const int bg_state) {
  cauto skyline = hpw::store_sprite->find("resource/image/loading logo/skyline.png");
  assert(skyline);
  insert_fast(dst, skyline->image());
}

void bgp_epilepsy(Image& dst, const int bg_state)
  { dst.fill( Pal8::from_real(rndr_fast(), rndu(3) == 0) ); }

void bgp_red_gradient(Image& dst, const int bg_state) {
  const real alpha_mul = 1.f / dst.Y;
  cauto gradient = (1.f + std::sin(bg_state * 0.009f)) * 0.5f;
  cauto a = Pal8::from_real(gradient);
  cauto b = Pal8::black;

  cfor(y, dst.Y) {
    cauto alpha = y * alpha_mul;
    cauto color = blend_alpha(a, b, alpha * 255.f);
    cfor(x, dst.X)
      dst(x, y) = color;
  }

  fast_dither_bayer16x16_4bit(dst, true);
  to_red(dst);
}

void bgp_red_gradient_2(Image& dst, const int bg_state) {
  const real alpha_mul = 1.f / dst.Y;
  cauto gradient = (1.f + std::sin(bg_state * 0.009f)) * 0.5f;
  cauto a = Pal8::from_real(gradient);
  cauto b = Pal8::black;

  cfor(y, dst.Y) {
    cauto alpha = y * alpha_mul;
    cauto color = blend_alpha(a, b, alpha * 255.f);
    cfor(x, dst.X)
      dst(x, y) = color;
  }

  dither_bayer16x16_1bit(dst);
  to_red(dst);
}

void bgp_repeated_rectangles(Image& dst, const int bg_state) {
  constexpr real space = 15.f;
  const real speed = bg_state * 0.015f;
  const real x = std::fmod(std::cos(speed) * 150.f, space);
  Rect rect(-x,-x, dst.X+x*2.f,dst.Y+x*2.f);

  dst.fill(Pal8::black);
  while (rect.size.x > 0 && rect.size.y > 0) {
    rect.pos += Vec(space, space);
    rect.size -= Vec(space*2.f, space*2.f);
    rect.pos = floor(rect.pos);
    rect.size = ceil(rect.size);
    draw_rect(dst, rect, Pal8::white);
    cfor (i, space/3)
      draw_rect (
        dst,
        Rect (
          rect.pos - Vec(i,i),
          rect.size + Vec(i,i)
        ),
        Pal8::white
      );
  }
} // bgp_repeated_rectangles

void bgp_tiles_2(Image& dst, const int bg_state) {
  // TODO
}

void bgp_tiles_1(Image& dst, const int bg_state) {
  // TODO
}

void bgp_circle_with_text(Image& dst, const int bg_state) {
  const int state = bg_state * 3;

  // фоновой текст
  Image text_layer(dst.X, dst.Y);
  bgp_unicode_red(text_layer, bg_state);

  // круг
  Image circle_layer(dst.X, dst.Y, Pal8::black);
  const real R = dst.Y/2.f - 25.f;
  draw_circle_filled(circle_layer,
    center_point(circle_layer), R, Pal8::white);
  
  // срезать текст кругом
  insert<&blend_and>(text_layer, circle_layer);

  // закрасить красным часть круга
  Image overlay_layer(dst.X, dst.Y);
  const int level = state % dst.Y;
  cfor(y, overlay_layer.Y) {
    // полоска движется вниз
    const bool cond_1 = level >= y;
    // когда полоска снизу, сверху убирается закрашивание
    const bool cond_2 = state % (dst.Y * 2) >= dst.Y;
    cfor(x, overlay_layer.X)
      overlay_layer(x, y) = cond_1 ^ cond_2
        ? Pal8::white : Pal8::black;
  }
  insert<&blend_and>(circle_layer, overlay_layer);
  to_red(circle_layer);
  insert_fast<&blend_max>(text_layer, circle_layer);

  insert_fast(dst, text_layer);
} // bgp_circle_with_text

void bgp_red_circle_black(Image& dst, const int bg_state) {
  dst.fill(Pal8::black);
  const real R = dst.Y/2.f - 25.f;
  draw_circle_filled(dst, center_point(dst), R, Pal8::red);
}

void bgp_red_circle_white(Image& dst, const int bg_state) {
  dst.fill(Pal8::white);
  const real R = dst.Y/2.f - 25.f;
  draw_circle_filled(dst, center_point(dst), R, Pal8::red);
}
