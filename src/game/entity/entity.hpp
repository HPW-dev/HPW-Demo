#pragma once
#include "status.hpp"
#include "entity-type.hpp"
#include "entity-animated.hpp"
#include "util/mem-types.hpp"
#include "util/mempool.hpp"
#include "util/str.hpp"
#include "game/entity/util/phys.hpp"

class Hitbox;

// игровая сущность
class Entity: public Entity_animated {
public:
  using Master = cp<Entity>;

  Phys phys {}; // физический контекст
  Uid uid {};
  mutable Entity_status status {}; // флаги
  Entity_type type {GET_SELF_TYPE};
  
  Entity();
  explicit Entity(Entity_type new_type);
  virtual ~Entity() = default;

  virtual void update(const Delta_time dt);
  void kill(); // убивает объект
  void remove(); // удаляет объект не вызывая взрыв и килл-колбэки
  void set_pos(const Vec pos);
  virtual void process_remove(); // вызывается только в Entity_mgr
  virtual void process_kill(); // вызывается только в Entity_mgr

  void set_master(Master new_master);
  inline cr<Master> get_master() const { return _master; }
  // узнать какой сейчас хитбокс
  inline virtual cp<Hitbox> get_hitbox() const { return {}; }
  
  inline cr<Str> name() const { return _name; }
  inline void set_name(cr<Str> new_name) { _name = new_name; }

private:
  nocopy(Entity);

  Str _name {}; // имя через которое соспавнили объект
  Master _master {}; // объект - создатель

  void move_it(const Delta_time dt);
}; // Entity
