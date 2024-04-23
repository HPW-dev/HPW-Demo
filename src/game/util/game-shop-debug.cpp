#include "game-shop-debug.hpp"
#include "graphic/image/image.hpp"

struct Shop_debug::Impl {
  inline Impl() {}

  inline void draw(Image& dst) const {

  }

  inline bool update(const double dt) {
    //return false;
    return true;
  }
};

Shop_debug::Shop_debug(): impl {new_unique<Impl>()} {}
Shop_debug::~Shop_debug() {}
void Shop_debug::draw(Image& dst) const { impl->draw(dst); }
bool Shop_debug::update(const double dt) { return impl->update(dt); }
