#pragma once
#include <functional>
#include "status.hpp"
#include "entity-type.hpp"
#include "util/macro.hpp"
#include "util/math/num-types.hpp"
#include "util/mem-types.hpp"
#include "util/mempool.hpp"
#include "game/entity/util/phys.hpp"
#include "game/entity/util/anim-ctx.hpp"

class Anim;
class Heat_distort;
class Image;
class Light;
class Hitbox;
struct Vec;

/// игровая сущность
class Entity {
  nocopy(Entity);

  using Update_callback = std::function<void (Entity&, double dt)>;
  Vector<Update_callback> update_callbacks {}; /// внешние колбэки на обработку апдейта

  void move_it(double dt);
  void draw_pos(Image& dst, const Vec offset) const;
  void debug_draw(Image& dst, const Vec offset) const;

public:
  Phys phys {}; /// физический контекст
  mutable Anim_ctx anim_ctx {}; /// анимация
  Shared<Heat_distort> heat_distort {}; /// эффект искажения воздуха
  Shared<Light> light {}; /// эффект вспышки
  using Master_p = CP<Entity>;
  Master_p master {}; /// объект создатель
  Uid uid {};
  mutable Enity_status status {}; /// флаги
  Entity_type type {GET_SELF_TYPE};
  
  Entity();
  explicit Entity(Entity_type new_type);
  virtual ~Entity() = default;

  virtual void draw(Image& dst, const Vec offset) const;
  virtual void update(double dt);
  virtual void kill();
  void set_pos(const Vec pos);
  void move_update_callback(Update_callback&& callback);
  void clear_callbacks();
  void set_master(Master_p new_master);
  inline CN<Master_p> get_master() const { return master; }
  /// узнать какой сейчас хитбокс
  inline virtual CP<Hitbox> get_hitbox() const { return {}; }
  /// безопасно получить свою анимацию
  CP<Anim> get_anim() const;

}; // Entity
