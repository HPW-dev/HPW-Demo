#pragma once
#include "ability-id.hpp"
#include "util/math/num-types.hpp"
#include "util/math/vec.hpp"
#include "graphic/image/image-fwd.hpp"

class Ability {
public:
  Ability() = default;
  virtual ~Ability() = default;
  virtual Ability_id id() const = 0;
  inline virtual void draw_bg(Image& dst, const Vec offset) {} // background
  inline virtual void draw_fg(Image& dst, const Vec offset) {} // foreground
  inline virtual void update(Delta_time dt) {}
  inline uint level() const { return _level; }

  inline virtual void on_upgrade() { ++_level; }
  inline virtual void on_downgrade() {
    if (_level > 0)
      --_level;
  }

protected:
  uint _level {1};
};
