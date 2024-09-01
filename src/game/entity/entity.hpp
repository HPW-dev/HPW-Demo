#pragma once
#include <functional>
#include "status.hpp"
#include "entity-type.hpp"
#include "util/macro.hpp"
#include "util/math/num-types.hpp"
#include "util/mem-types.hpp"
#include "util/mempool.hpp"
#include "util/str.hpp"
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
  using Update_cbs = std::function<void (Entity&, const Delta_time)>;
  // <self ptr, dst, offset>
  using Draw_bg_cb = std::function<void (cr<Entity>, Image&, const Vec)>;
  using Draw_fg_cb = Draw_bg_cb;
  // <self ptr>
  using Kill_cb = std::function<void (Entity&)>;
  using Remove_cb = Kill_cb;

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

  #define MAKE_CB_DEF(FNAME, TYPE) \
    void FNAME(cr<TYPE> callback); \
    void FNAME(TYPE&& callback);
  MAKE_CB_DEF(add_update_cb, Update_cbs)
  MAKE_CB_DEF(add_kill_cb, Kill_cb)
  MAKE_CB_DEF(add_draw_bg_cb, Draw_bg_cb)
  MAKE_CB_DEF(add_draw_fg_cb, Draw_fg_cb)
  MAKE_CB_DEF(add_remove_cb, Remove_cb)
  #undef MAKE_CB_DEF

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
  Vector<Update_cbs> _update_cbs {}; // внешние колбэки на обработку апдейта
  Vector<Kill_cb> _kill_cbs {}; // внешние колбэки на обработку смерти
  Vector<Remove_cb> _remove_cbs {}; // колюэки при удалении из системы
  Vector<Draw_bg_cb> _draw_bg_cbs {}; // вызываются при отрисовке под объектом
  Vector<Draw_fg_cb> _draw_fg_cbs {}; // вызываются при отрисовке над объектом

  void move_it(const Delta_time dt);
  void draw_pos(Image& dst, const Vec offset) const;
  void debug_draw(Image& dst, const Vec offset) const;
  void draw_haze(Image& dst, const Vec offset) const;

  void process_kill_cbs();
  void process_update_cbs(Delta_time dt);
  void process_remove_cbs();
  void process_draw_fg_cbs(Image& dst, const Vec offset) const;
  void process_draw_bg_cbs(Image& dst, const Vec offset) const;
}; // Entity
