#include "glass-ball.hpp"
#include "graphic/image/image.hpp"

struct Glass_ball::Impl {
  inline Impl() {

  }

  inline void update(Delta_time dt) {

  }

  inline void draw(Image& dst) const {

  }
}; // Impl

Glass_ball::Glass_ball(): _impl{new_unique<Impl>()} {}
Glass_ball::~Glass_ball() {}
void Glass_ball::update(Delta_time dt) { _impl->update(dt); }
void Glass_ball::draw(Image& dst) const { _impl->draw(dst); }