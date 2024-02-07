#pragma once
#include "entity.hpp"

/// частица, ни с чем не сталкивается
class Particle final: public Entity {
  nocopy(Particle);
  
  double lifetime {}; /// время жизни частицы

  void lifetime_update(double dt);

public:
  Particle();
  ~Particle() = default;

  void draw(Image& dst, const Vec offset) const override;
  void update(double dt) override;
  void set_lifetime(double new_lifetime, bool enable_flag=true);
};
