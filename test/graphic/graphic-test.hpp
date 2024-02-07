#pragma once
#include <vector>
#include <functional>
#include "host/host-glfw.hpp"
#include "util/unicode.hpp"

class Timer;
class Ball;
class Image;

/// тесты модулей графики
void graphic_tests();

class Graphic_test final: public Host_glfw {
  real tmr {}; /// state timer
  Shared<Timer> bg_tmr {}; /// backgr skit timer
  Shared<Timer> board_2_tmr {}; /// по истечению таймера заливка чёрным
  uint cur_bg {}; /// cur backgr id
  using bg_f = void (Graphic_test::*)(Image& dst) const;
  Vector<bg_f> v_bg {}; /// backgr printer funcs
  Shared<Ball> ball_1 {};
  Shared<Ball> ball_2 {};
  Shared<Image> strawberry {};
  Shared<Image> board_2 {};
  real pos_1 {}, pos_2 {}, pos_3 {}, pos_4 {}; /// for board 2
  utf32 text {};

  void next_gr();
  void prev_gr();
  void draw_bit_effect_1(Image& dst) const;
  void draw_bit_effect_2(Image& dst) const;
  void draw_bit_effect_3(Image& dst) const;
  void draw_noise(Image& dst) const;
  void draw_insert(Image& dst) const;
  void draw_gradient(Image& dst) const;
  void draw_flickr(Image& dst) const;
  void draw_board_1(Image& dst) const;
  void update_board_2();
  void draw_board_2(Image& dst) const; /// кружки с dt
  void draw_filled(Image& dst) const;
  void draw_font(Image& dst) const;
  void overdraw(Image& dst) const; /// рисует по углам вне пределах канваса
  void draw_fps(Image& dst) const;
  void init() override;
  void update(double dt) override;

public:
  explicit Graphic_test(int argc, char *argv[]);
  ~Graphic_test() = default;
  void draw() override;
}; // Graphic_test
