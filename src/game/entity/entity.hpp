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

// игровая сущность
class Entity {
public:
  // <self ptr, dt>
  using Update_callback = std::function<void (Entity&, const Delta_time)>;
  // <self ptr>
  using Kill_callback = std::function<void (Entity&)>;

private:
  nocopy(Entity);
  Vector<Update_callback> update_callbacks {}; // внешние колбэки на обработку апдейта
  Vector<Kill_callback> kill_callbacks {}; // внешние колбэки на обработку смерти

  void move_it(const Delta_time dt);
  void draw_pos(Image& dst, const Vec offset) const;
  void debug_draw(Image& dst, const Vec offset) const;

public:
  Phys phys {}; // физический контекст
  mutable Anim_ctx anim_ctx {}; // анимация
  Shared<Heat_distort> heat_distort {}; // эффект искажения воздуха
  Shared<Light> light {}; // эффект вспышки
  using Master_p = CP<Entity>;
  Master_p master {}; // объект создатель
  Uid uid {};
  mutable Enity_status status {}; // флаги
  Entity_type type {GET_SELF_TYPE};
  
  Entity();
  explicit Entity(Entity_type new_type);
  virtual ~Entity() = default;

  virtual void draw(Image& dst, const Vec offset) const;
  virtual void update(const Delta_time dt);
  virtual void kill();
  void set_pos(const Vec pos);
  void add_update_callback(CN<Update_callback> callback);
  void move_update_callback(Update_callback&& callback);
  void move_kill_callback(Kill_callback&& callback);
  void accept_kill_callbacks();
  void clear_callbacks();
  void set_master(Master_p new_master);
  inline CN<Master_p> get_master() const { return master; }
  // узнать какой сейчас хитбокс
  inline virtual CP<Hitbox> get_hitbox() const { return {}; }
  // безопасно получить свою анимацию
  CP<Anim> get_anim() const;

}; // Entity
