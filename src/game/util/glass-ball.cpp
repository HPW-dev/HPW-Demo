#include <cassert>
#include <cmath>
#include "glass-ball.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/util-templ.hpp"
#include "game/core/sprites.hpp"
#include "game/core/canvas.hpp"
#include "game/core/fonts.hpp"
#include "game/util/vec-helper.hpp"
#include "util/hpw-util.hpp"

class Glass_ball::Impl {
  Shared<Sprite> _spr {};
  Vec _pos {};
  Vec _vel {};
  int _W {};
  int _H {};
  real _start_speed {9.0_pps};  // с какой скоростью пулять шар
  real _air_force {1.2};        // сопротивление воздуха
  real _g {9.81_pps};           // ускорение падения
  real _mass {1};               // массша шара
  real _bound_force {0.3};      // замедление шара при столкновении с краями экрана
  real _ground_force {0.1};     // трение об землю

  inline bool _process_bounds() {
    bool collided = false;
    if (_pos.x < 0) {
      _pos.x *= -1;
      _vel.x *= -(1.0 - _bound_force);
      collided = true;
    }
    if (_pos.x >= _W) {
      _pos.x = _W - (_pos.x - _W);
      _vel.x *= -(1.0 - _bound_force);
      collided = true;
    }
    if (_pos.y < 0) {
      _pos.y = -1;
      _vel.y *= -(1.0 - _bound_force);
      collided = true;
    }
    if (_pos.y >= _H) {
      _pos.y = _H - (_pos.y - _H);
      _vel.y *= -(1.0 - _bound_force);
      // шар трётся об землю
      if (std::abs(_vel.y) <= 1)
        _vel.x *= (1.0 - _ground_force);
      collided = true;
    }
    return collided;
  }

  inline void _respawn() {
    _pos = get_rand_pos_graphic(0, 0, _W, _H);
    _vel = rand_normalized_graphic() * _start_speed * _mass;
  }

public:
  inline Impl() {
    _spr = hpw::sprites.find("resource/image/other/red glass ball.png");
    assert(_spr);
    _W = graphic::width - _spr->X();
    _H = graphic::height - _spr->Y();
    _respawn();
  }

  inline void update(Delta_time dt) {
    const Vec FORCE_AIR = _vel * _air_force * -1;
    const Vec FORCE_GRAVITY(0, _g * _mass);
    const Vec ACCEL = (FORCE_AIR + FORCE_GRAVITY) / _mass;
    _vel += ACCEL * dt;
    _pos += _vel * dt;
    _process_bounds();
  }

  inline void draw(Image& dst) const {
    assert(dst);
    insert(dst, *_spr, _pos);
    graphic::font->draw(dst, {5, 5}, U"vel: " + n2s<utf32>(_vel.x) + U", " + n2s<utf32>(_vel.y));
  }
}; // Impl

Glass_ball::Glass_ball(): _impl{new_unique<Impl>()} {}
Glass_ball::~Glass_ball() {}
void Glass_ball::update(Delta_time dt) { _impl->update(dt); }
void Glass_ball::draw(Image& dst) const { _impl->draw(dst); }