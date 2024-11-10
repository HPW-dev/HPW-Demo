#include "shop.hpp"

struct Shop_task::Impl {
  inline Impl() = default;
  inline void update(const Delta_time dt) {}
  inline void draw(Image& dst) const {}
};

Shop_task::Shop_task(): impl{new_unique<Impl>()} {}
Shop_task::~Shop_task() {}
void Shop_task::update(const Delta_time dt) { impl->update(dt); }
void Shop_task::draw(Image& dst) const { impl->draw(dst); }
