#include <list>
#include <cassert>
#include <utility>
#include <algorithm>
#include "post-effects.hpp"
#include "graphic/image/image.hpp"

struct Effect_mgr::Impl {
  std::list<Shared<Effect>> effects {}; // эффекты постобработки

  inline void move_to_front(Shared<Effect>&& effect)
    { effects.emplace_front(std::move(effect)); }

  inline void move_to_back(Shared<Effect>&& effect)
    { effects.emplace_back(std::move(effect)); }

  inline void update(const Delta_time dt) {
    // обновить эффект и удалить его, если он завершился
    // при update ret true будет удаление эффекта
    std::erase_if(effects,
      [dt] (nauto effect) { return effect->update(dt); });
  }

  inline void draw(Image& dst) const {
    for (cnauto effect: effects)
      effect->draw(dst);
  }
}; // Effect_mgr::Impl

Effect_mgr::Effect_mgr(): impl {new_unique<Impl>()} {}
Effect_mgr::~Effect_mgr() {}
void Effect_mgr::move_to_front(Shared<Effect>&& effect) { impl->move_to_front(std::move(effect)); }
void Effect_mgr::move_to_back(Shared<Effect>&& effect) { impl->move_to_back(std::move(effect)); }
void Effect_mgr::update(const Delta_time dt) { impl->update(dt); }
void Effect_mgr::draw(Image& dst) const { impl->draw(dst); }
