#include "level-debug-1.hpp"
#include "graphic/image/image.hpp"
#include "graphic/font/font.hpp"
#include "graphic/util/util-templ.hpp"
#include "game/core/canvas.hpp"
#include "game/core/fonts.hpp"
#include "game/entity/util/phys.hpp"
#include "util/hpw-util.hpp"
#include "util/math/random.hpp"

Level_debug_1::Level_debug_1() {
  set_rnd_seed(97'988u); // чтобы тест всегда выглядел одинаково

  // случайная точка в пределах экрана
  auto rnd_pos = []{
    return Vec{
      rndr(0, graphic::width),
      rndr(0, graphic::height)
    };
  };

  { // летает с ускорением
    auto obj = new_shared<Phys>();
    obj->set_pos(rnd_pos());
    obj->set_vel(rand_normalized_stable() * 0.1_pps);
    obj->set_accel(1_pps);
    objs.emplace_back(obj);
  }

  { // неподвижный
    auto obj = new_shared<Phys>();
    obj->set_pos(rnd_pos());
    objs.emplace_back(obj);
  }

  { // медленно летает
    auto obj = new_shared<Phys>();
    obj->set_pos(rnd_pos());
    obj->set_vel(rand_normalized_stable() * 0.25_pps);
    objs.emplace_back(obj);
  }

  { // быстро летает с замедлением
    auto obj = new_shared<Phys>();
    obj->set_pos(rnd_pos());
    obj->set_vel(rand_normalized_stable() * 50_pps);
    obj->set_force(0.75);
    objs.emplace_back(obj);
  }

  { // летает и поворачивается с постоянной скоростью
    auto obj = new_shared<Phys>();
    obj->set_pos(rnd_pos());
    obj->set_vel(rand_normalized_stable() * 4_pps);
    obj->set_rot_spd(1_pps);
    objs.emplace_back(obj);
  }

  { // летает и поворачивается в противоположную сторону с ускорением
    auto obj = new_shared<Phys>();
    obj->set_pos(rnd_pos());
    obj->set_vel(rand_normalized_stable() * 4_pps);
    obj->set_rot_spd(-0.25_pps);
    obj->set_rot_ac(3_pps);
    objs.emplace_back(obj);
  }

  { // летает и замедляет своё вращение
    auto obj = new_shared<Phys>();
    obj->set_pos(rnd_pos());
    obj->set_vel(rand_normalized_stable() * 2_pps);
    obj->set_rot_spd(20_pps);
    obj->set_rot_fc(0.4);
    objs.emplace_back(obj);
  }
} // c-tor

void Level_debug_1::update(const Vec vel, double dt) {
  for (nauto obj: objs) {
    obj->update(dt);
    bound_check(*obj);
  }
}

void Level_debug_1::draw(Image& dst) const {
  draw_bg(dst);
  for (nauto obj: objs)
    draw_obj(*obj, dst);
} // draw

void Level_debug_1::draw_bg(Image& dst) const {
  dst.fill(Pal8::black);
  cfor (y, dst.Y)
  cfor (x, dst.X) {
    auto grid_sz = 32;
    auto grid_color = Pal8::from_real(0.2);
    if ((x % grid_sz == 0) || (y % grid_sz == 0))
      dst(x, y) = grid_color;
  }
}

void Level_debug_1::draw_obj(CN<Phys> obj, Image& dst) const {
  auto color = Pal8::white;
  real direct_len = 60; // длинна стрелки показывающей вектор движения
  auto pos = obj.get_pos();
  auto accel = obj.get_accel();
  auto degree = obj.get_deg();
  auto speed = obj.get_speed();
  //auto direct_deg = deg_to_vec(degree);
  auto force = obj.get_force();
  auto vel = obj.get_vel();
  auto direct = normalize_graphic(vel);
  auto rotate_accel = obj.get_rot_ac();
  auto rotate_force = obj.get_rot_fc();
  auto rotate_speed = obj.get_rot_spd();

  // draw pos
  draw_cross(dst, pos, color, 4);
  draw_circle(dst, pos, 12, color);

  // draw info
  utf32 info;
  #define n2u(val) n2s<utf32>(val, 1)
  info += U"pos: " + n2u(pos.x) + U", " + n2u(pos.y) + U'\n';
  info += U"speed: " + n2s<utf32>(speed, 3) + U'\n';
  info += U"accel: " + n2s<utf32>(accel, 6) + U'\n';
  info += U"force: " + n2s<utf32>(force, 2) + U'\n';
  info += U"degree: " + n2u(degree) + U'\n';
  info += U"vel: " + n2s<utf32>(vel.x, 3) + U", " + n2s<utf32>(vel.y, 3) + U'\n';
  info += U"rot ac: " +n2s<utf32>(rotate_accel, 3) + U'\n';
  info += U"rot fc: " +n2s<utf32>(rotate_force, 3) + U'\n';
  info += U"rot spd: " +n2s<utf32>(rotate_speed, 3) + U'\n';
  #undef n2u
  graphic::font->draw(dst, pos + Vec(0, 14), info);

  // draw direction vector
  draw_line<&blend_diff>(dst, pos, pos + direct * direct_len, Pal8::white);
  //draw_line<&blend_diff>(dst, pos, pos + direct_deg * direct_len, Pal8::red);
} // draw_obj

void Level_debug_1::bound_check(Phys& obj) const {
  auto pos = obj.get_pos();
  auto vel = obj.get_vel();
  if (pos.x < 0) {
    pos.x = 0;
    vel.x *= -1;
  }
  if (pos.x >= graphic::width) {
    pos.x = graphic::width-1;
    vel.x *= -1;
  }
  if (pos.y < 0) {
    pos.y = 0;
    vel.y *= -1;
  }
  if (pos.y >= graphic::height) {
    pos.y = graphic::height-1;
    vel.y *= -1;
  }
  obj.set_pos(pos);
  obj.set_vel(vel);
} // bound_check
