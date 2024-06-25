#include <omp.h>
#include <cassert>
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

void bgp_spline(Image& dst, const int bg_state) {
  // TODO
}

void bgp_circle_with_text(Image& dst, const int bg_state) {
  // TODO
}

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
  const real state = bg_state * 0.0005f;
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
  const real state = bg_state * 0.0005f;
  const real mul_x = 1.0 / dst.X;

  #pragma omp parallel for simd collapse(2)
  cfor (y, dst.Y)
  cfor (x, dst.X) {
    const Pal8 color = std::fmod(y + x + std::cos(state) * 100'000.0f, 40.f) > 20.f
      ? Pal8::from_real(x * mul_x, true) : Pal8{};
    dst(x, y) = color;
  }
}

void bgp_tiles_1(Image& dst, const int bg_state) {
  // TODO
}

void bgp_skyline(Image& dst, const int bg_state) {
  cauto skyline = hpw::store_sprite->find("resource/image/loading logo/skyline.png");
  assert(skyline);
  insert_fast(dst, skyline->image());
}

void bgp_tiles_2(Image& dst, const int bg_state) {
  // TODO
}
