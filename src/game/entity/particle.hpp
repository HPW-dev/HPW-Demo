#pragma once
#include "entity.hpp"

// частица, ни с чем не сталкивается
class Particle final: public Entity {
  nocopy(Particle);
  
  Delta_time lifetime {}; // время жизни частицы

  void lifetime_update(const Delta_time dt);

public:
  Particle();
  ~Particle() = default;

  void draw(Image& dst, const Vec offset) const override;
  void update(const Delta_time dt) override;
  void set_lifetime(Delta_time new_lifetime, bool enable_flag=true);
};
