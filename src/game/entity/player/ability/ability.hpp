#pragma once
#include "ability-id.hpp"
#include "util/math/num-types.hpp"

class Image;

class Ability {
public:
  Ability() = default;
  virtual ~Ability() = default;
  virtual Ability_id id() const = 0;
  inline virtual void draw_bg(Image& dst) {} // background
  inline virtual void draw_fg(Image& dst) {} // foreground
  inline virtual void update(Delta_time dt) {}
  inline virtual void on_give() {}
  inline virtual void on_remove() {}
  inline virtual void on_upgrade() {}
  inline virtual void on_downgrade() {}
  inline uint level() const { return _level; }

protected:
  uint _level {};
};
