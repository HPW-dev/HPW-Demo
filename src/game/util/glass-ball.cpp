#include <cassert>
#include "glass-ball.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/util-templ.hpp"
#include "game/core/sprites.hpp"
#include "game/core/canvas.hpp"
#include "util/hpw-util.hpp"

class Glass_ball::Impl {
  Shared<Sprite> _spr {};
  Vec _pos {};
  Vec _vel {};
  real _start_speed {9.0_pps};  // с какой скоростью пулять шар
  real _air_force {0.5};        // сопротивление воздуха
  real _g {9.0_pps};            // ускорение падения
  real _mass {40};              // массша шара
  real _bound_force {0.4};      // замедление шара при столкновении с краями экрана

  inline bool _process_bounds() {
    cauto W = graphic::width - _spr->X();
    cauto H = graphic::height - _spr->Y();
    bool collided = false;
    if (_pos.x < 0) {
      _pos.x *= -1;
      _vel.x *= -(1.0 - _bound_force);
      collided = true;
    }
    if (_pos.x >= W) {
      _pos.x = W - (_pos.x - W);
      _vel.x *= -(1.0 - _bound_force);
      collided = true;
    }
    if (_pos.y < 0) {
      _pos.y = -1;
      _vel.y *= -(1.0 - _bound_force);
      collided = true;
    }
    if (_pos.y >= H) {
      _pos.y = H - (_pos.y - H);
      _vel.y *= -(1.0 - _bound_force);
      collided = true;
    }
    return collided;
  }

public:
  inline Impl() {
    _spr = hpw::sprites.find("resource/image/other/red glass ball.png");
    assert(_spr);
  }

  inline void update(Delta_time dt) {
    Vec accel(0, _mass * _g);
    accel += _vel * _air_force * -1;
    _vel += (accel / _mass) * dt;
    _pos += _vel * dt;
    _process_bounds();
  }

  inline void draw(Image& dst) const {
    assert(dst);
    insert(dst, *_spr, _pos);
  }
}; // Impl

Glass_ball::Glass_ball(): _impl{new_unique<Impl>()} {}
Glass_ball::~Glass_ball() {}
void Glass_ball::update(Delta_time dt) { _impl->update(dt); }
void Glass_ball::draw(Image& dst) const { _impl->draw(dst); }