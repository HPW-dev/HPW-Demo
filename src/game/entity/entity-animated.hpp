#pragma once
#include "entity-debug.hpp"
#include "entity-cbs.hpp"
#include "util/macro.hpp"
#include "game/entity/util/anim-ctx.hpp"

class Entity;
class Image;
struct Vec;
class Anim;
class Heat_distort;
class Light;

// даёт игровым объектам анимацию и эффекты
class Entity_animated: public Entity_debug, public Entity_cbs {
public:
  mutable Anim_ctx anim_ctx {}; // анимация
  Shared<Heat_distort> heat_distort {}; // эффект искажения воздуха
  Shared<Light> light {}; // эффект вспышки

  inline explicit Entity_animated(Entity& master)
    : Entity_debug (master)
    , Entity_cbs (master)
    , _master {master}
    {}
  // безопасно получить свою анимацию
  cp<Anim> get_anim() const;
  virtual void draw(Image& dst, const Vec offset) const;

protected:
  // рисует искажение воздуха
  void draw_haze(Image& dst, const Vec offset) const;
  void update(const Delta_time dt);

private:
  Entity& _master;
  nocopy(Entity_animated);
};
