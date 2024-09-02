#pragma once
#include "entity-cbs.hpp"
#include "status.hpp"
#include "entity-type.hpp"
#include "util/mem-types.hpp"
#include "util/mempool.hpp"
#include "util/str.hpp"
#include "game/entity/util/phys.hpp"
#include "game/entity/util/anim-ctx.hpp"

class Anim;
class Heat_distort;
class Light;
class Hitbox;

// игровая сущность
class Entity: public Entity_cbs {
public:
  Phys phys {}; // физический контекст
  mutable Anim_ctx anim_ctx {}; // анимация
  Shared<Heat_distort> heat_distort {}; // эффект искажения воздуха
  Shared<Light> light {}; // эффект вспышки
  using Master_p = cp<Entity>;
  Master_p master {}; // объект создатель
  Uid uid {};
  mutable Entity_status status {}; // флаги
  Entity_type type {GET_SELF_TYPE};
  
  Entity();
  explicit Entity(Entity_type new_type);
  virtual ~Entity() = default;

  virtual void draw(Image& dst, const Vec offset) const;
  virtual void update(const Delta_time dt);
  void kill(); // убивает объект
  void remove(); // удаляет объект не вызывая взрыв и килл-колбэки
  void set_pos(const Vec pos);
  virtual void process_remove(); // вызывается только в Entity_mgr
  virtual void process_kill(); // вызывается только в Entity_mgr

  void set_master(Master_p new_master);
  inline cr<Master_p> get_master() const { return master; }
  // узнать какой сейчас хитбокс
  inline virtual cp<Hitbox> get_hitbox() const { return {}; }
  // безопасно получить свою анимацию
  cp<Anim> get_anim() const;
  inline cr<Str> name() const { return _name; }
  inline void set_name(cr<Str> new_name) { _name = new_name; }

private:
  nocopy(Entity);

  Str _name {}; // имя, через которое соспавнили объект

  void move_it(const Delta_time dt);
  void draw_pos(Image& dst, const Vec offset) const;
  void debug_draw(Image& dst, const Vec offset) const;
  void draw_haze(Image& dst, const Vec offset) const;
}; // Entity
