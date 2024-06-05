#include <cmath>
#include <ctime>
#include "graphic-test.hpp"
#include "graphic/util/graphic-util.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/image/image-io.hpp"
#include "graphic/font/unifont.hpp"
#include "util/error.hpp"
#include "util/hpw-util.hpp"
#include "util/log.hpp"
#include "game/core/canvas.hpp"
#include "game/core/core.hpp"
#include "game/core/fonts.hpp"
#include "game/core/graphic.hpp"
#include "game/core/common.hpp"
#include "game/util/sync.hpp"
#include "game/util/keybits.hpp"
#include "game/util/game-util.hpp"
#include "game/util/game-archive.hpp"
#include "host/command.hpp"
#include "host/host-util.hpp"
#include "util/math/timer.hpp"
#include "util/math/random.hpp"
#include "ball.hpp"

Graphic_test::Graphic_test(int argc, char *argv[])
: Host_glfw(argc, argv) {}

void Graphic_test::init() {
  Host_glfw::init();
  graphic_tests();
  hpw_log("Gpragic test usage: shoot back key - VSync\n");
  bg_tmr = new_shared<Timer>(5.0);
  board_2_tmr = new_shared<Timer>(0.5);
  // добавление фонов
  v_bg.push_back(&Graphic_test::draw_bit_effect_1);
  v_bg.push_back(&Graphic_test::draw_bit_effect_2);
  v_bg.push_back(&Graphic_test::draw_gradient);
  v_bg.push_back(&Graphic_test::draw_bit_effect_3);
  v_bg.push_back(&Graphic_test::draw_flickr);
  v_bg.push_back(&Graphic_test::draw_filled);
  v_bg.push_back(&Graphic_test::draw_noise);
  v_bg.push_back(&Graphic_test::draw_insert);
  v_bg.push_back({});
  // шарики
  ball_1 = new_shared<Ball>(1_pps, Pal8::red);
  ball_2 = new_shared<Ball>(4_pps, Pal8::white);
  // первый фон случайный
  cur_bg = rndu(v_bg.size() - 1);
  strawberry = new_shared<Image>();
  load(*strawberry, hpw::cur_dir + "../test/graphic/strawberry.png");
  board_2 = new_shared<Image>(graphic::canvas->X - 30, 40);
  text = U"EN: ABCDEFG abcdefg\n"
    U"RU: АБВГДЖЗ абвгджз\n"
    U"JP: 漢字 仮借文字\n"
    U"CH: 是我这来中\n"
    U"Symbols: 🍓+-/\\=:;.,|*&^%$#@!?\n"
    U"Nums: 0123456789\n";
  auto font_mem {hpw::archive->get_file("resource/font/unifont-13.0.06.ttf")};
  graphic::font = new_shared<Unifont>(font_mem, 16, false);
} // init

void Graphic_test::update(const Delta_time dt) {
  tmr += dt;
  // опрос клавы
  if (is_pressed_once(hpw::keycode::escape))
    hpw::soft_exit();
  
  // переключает VSync
  static bool fps_lock = graphic::get_vsync();
  if (is_pressed_once(hpw::keycode::shoot)) {
    fps_lock = !fps_lock;
    detailed_log("VSync:" << s2yn(fps_lock) << std::endl);
    graphic::set_disable_frame_limit(fps_lock);
    graphic::wait_frame = fps_lock;
  }

  if (is_pressed_once(hpw::keycode::left)) {
    prev_gr();
    bg_tmr->reset();
  }
  if (is_pressed_once(hpw::keycode::right)) {
    next_gr();
    bg_tmr->reset();
  }
  // обновить таймер фона
  if (bg_tmr->update(hpw::real_dt))
    next_gr();
  ball_1->move(dt);
  ball_2->move(dt);
  update_board_2();
} // update

void Graphic_test::draw() {
  return_if ( !graphic::enable_render);
  nauto dst {*graphic::canvas};

  // draw backgrounds
  if (auto pf = v_bg[cur_bg]; pf)
    (this->*pf)(dst);
  ball_2->draw(dst);
  draw_board_1(dst);
  draw_board_2(dst);
  draw_font(dst);
  overdraw(dst);
  ball_1->draw(dst);
  draw_fps(dst);
  Host_glfw::draw();
} // draw

void Graphic_test::next_gr() { cur_bg = (cur_bg + 1) % v_bg.size(); }

void Graphic_test::prev_gr() { 
  if (cur_bg == 0) {
    cur_bg = v_bg.size() - 1;
    return;
  }
  cur_bg = (cur_bg - 1) % v_bg.size();
}

void Graphic_test::draw_bit_effect_1(Image& dst) const {
  int v = pps(tmr);
  cfor (y, dst.Y)
  cfor (x, dst.X) {
    int pix = (x + v) ^ (y + v);
    pix |= v;
    pix >>= 4;
    pix <<= 4;
    dst(x, y) = pix;
  }
}

void Graphic_test::draw_bit_effect_2(Image& dst) const {
  int v = pps(tmr);
  cfor (y, dst.Y)
  cfor (x, dst.X) {
    int pix = (x - v) & (y + v);
    pix += v;
    dst(x, y) = pix;
  }
}

void Graphic_test::draw_gradient(Image& dst) const {
  int v = pps(tmr * 0.5);
  cfor (y, dst.Y)
  cfor (x, dst.X) {
    int pix = y;
    pix += v;
    dst(x, y) = pix;
  }
}

void Graphic_test::draw_noise(Image& dst) const {
  for (nauto pix: dst)
    pix.set(rndu_fast());
}

void Graphic_test::draw_bit_effect_3(Image& dst) const {
  int v = pps(tmr);
  cfor (y, dst.Y)
  cfor (x, dst.X) {
    int pix = ((x + v) >> 2) * ((y - v) >> 2);
    int tmp = pix << 4;
    tmp |= v;
    pix &= v;
    pix ^= tmp;
    int pix2 = (x + v) ^ (y - v);
    tmp = pix2 >> 2;
    tmp |= v;
    pix2 &= v;
    pix2 ^= tmp;
    pix &= pix2;
    dst(x, y) = pix;
  }
} // draw_bit_effect_3

void Graphic_test::draw_flickr(Image& dst) const {
  static bool inv;
  inv = !inv;
  auto pix = Pal8(inv ? Pal8::black : Pal8::white);
  cfor (y, dst.Y) {
    for (int i = 2; i < 5; ++i)
      if (y % i == 0)
        pix = ~pix;
    cfor (x, dst.X)
      dst(x, y) = pix;
  }
} // draw_bit_effect_3

void Graphic_test::draw_board_1(Image& dst) const {
  constexpr int bound = 10;
  auto X = strawberry->X;
  auto Y = strawberry->Y;
  draw_rect_filled<&blend_158>(dst, {bound, bound,
    dst.X - bound*2, Y*4 + 10}, Pal8::black);
  #define COPY(x, y, bf) insert<bf>(dst, *strawberry, {bound+6+X*x, bound+6+Y*y})
  COPY(0, 0, &blend_or);
  COPY(1, 0, &blend_sub);
  COPY(2, 0, &blend_add);
  COPY(3, 0, &blend_mul);
  COPY(4, 0, &blend_and);
  COPY(5, 0, &blend_min);
  COPY(6, 0, &blend_max);
  COPY(7, 0, &blend_avr);
  COPY(8, 0, &blend_158);
  COPY(9, 0, &blend_past);
  COPY(0, 1, &blend_diff);
  COPY(1, 1, &blend_overlay);
  COPY(2, 1, &blend_or_safe);
  COPY(3, 1, &blend_add_safe);
  COPY(4, 1, &blend_sub_safe);
  COPY(5, 1, &blend_mul_safe);
  COPY(6, 1, &blend_and_safe);
  COPY(7, 1, &blend_softlight);
  COPY(8, 1, &blend_xor);
  COPY(9, 1, &blend_xor_safe);
  #undef COPY
  #define BLINK(x, y, bf) insert_blink<bf>(dst, *strawberry, {bound+6+X*x, bound+6+Y*y}, graphic::frame_count);
  BLINK(0, 2, &blend_or);
  BLINK(1, 2, &blend_sub);
  BLINK(2, 2, &blend_add);
  BLINK(3, 2, &blend_mul);
  BLINK(4, 2, &blend_and);
  BLINK(5, 2, &blend_min);
  BLINK(6, 2, &blend_max);
  BLINK(7, 2, &blend_avr);
  BLINK(8, 2, &blend_158);
  BLINK(9, 2, &blend_past);
  BLINK(0, 3, &blend_diff);
  BLINK(1, 3, &blend_overlay);
  BLINK(2, 3, &blend_or_safe);
  BLINK(3, 3, &blend_add_safe);
  BLINK(4, 3, &blend_sub_safe);
  BLINK(5, 3, &blend_mul_safe);
  BLINK(6, 3, &blend_and_safe);
  BLINK(7, 3, &blend_softlight);
  BLINK(8, 3, &blend_xor);
  BLINK(9, 3, &blend_xor_safe);
  #undef BLINK
} // draw_board_1

void Graphic_test::update_board_2() {
  auto ring_move = [](real& dst, real speed)
    { dst = std::fmod(dst + speed * hpw::safe_dt, graphic::canvas->X); };
  ring_move(pos_1, 1.0);
  ring_move(pos_2, 60.0);
  ring_move(pos_3, 240.0);
  ring_move(pos_4, 1000.0);
  cfor (_, board_2_tmr->update(hpw::safe_dt)) {
    board_2->fill(Pal8::black);
  }
  auto ratio = 1.0 - board_2_tmr->ratio();
  auto red_col = Pal8::from_real(ratio, true);
  auto gray_col = Pal8::from_real(ratio);
  auto radius = 4;
  draw_circle_filled(*board_2, {pos_1, 5+(radius+1)*2*0}, radius, red_col);
  draw_circle_filled(*board_2, {pos_2, 5+(radius+1)*2*1}, radius, gray_col);
  draw_circle_filled(*board_2, {pos_3, 5+(radius+1)*2*2}, radius, red_col);
  draw_circle_filled(*board_2, {pos_4, 5+(radius+1)*2*3}, radius, gray_col);
} // update_board_2

void Graphic_test::draw_board_2(Image& dst) const
  { insert(dst, *board_2, {15, dst.Y - board_2->Y - 15}); }

void Graphic_test::overdraw(Image& dst) const {
  auto L = -strawberry->X / 2;
  auto R = dst.X - strawberry->X / 2;
  auto U = -strawberry->Y / 2;
  auto D = dst.Y - strawberry->Y / 2;
  insert_interlace(dst, *strawberry, {L, U}, graphic::frame_count);
  insert_interlace(dst, *strawberry, {R, U}, graphic::frame_count);
  insert_interlace(dst, *strawberry, {L, D}, graphic::frame_count);
  insert_interlace(dst, *strawberry, {R, D}, graphic::frame_count);
}

void Graphic_test::draw_filled(Image& dst) const
  { dst.fill(graphic::frame_count >> 1); }

void Graphic_test::draw_font(Image& dst) const {
  auto weidth = graphic::font->text_width(text);
  draw_rect_filled<blend_158>(dst,
    Rect(10, 217, weidth, 104), Pal8::black);
  graphic::font->draw(dst, {11, 224}, text, &blend_max);
  graphic::font->draw(dst, {11 + weidth + 7, 224}, text, blend_diff);
}

void Graphic_test::draw_fps(Image& dst) const {
  auto fps = U" FPS:" + n2s<utf32>(graphic::cur_fps, 1) +
    U" DT:" + n2s<utf32>(hpw::safe_dt, 7);
  auto h = graphic::font->text_height(fps);
  auto w = graphic::font->text_width(fps);
  constexpr auto xoff = 5;
  constexpr auto yoff = 5;
  draw_rect_filled(dst, {xoff, yoff, w, h}, Pal8::black);
  graphic::font->draw(dst, {xoff+1, yoff+1}, fps, &blend_past);
}

void Graphic_test::draw_insert(Image& dst) const {
  cnauto src = *strawberry.get();
  cfor (x, 25'000)
    insert(dst, src, {
      rnd(-src.X, graphic::canvas->X + src.X),
      rnd(-src.Y, graphic::canvas->Y + src.Y)
    });
}
