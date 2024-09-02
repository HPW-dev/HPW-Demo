#pragma once
#include <functional>
#include "util/macro.hpp"
#include "util/vector-types.hpp"
#include "util/math/num-types.hpp"

class Entity;
class Image;
struct Vec;

// управляет колбэками игровых объектов
class Entity_cbs {
public:
  Entity& _master;

  // <self ptr, dt>
  using Update_cb = std::function<void (Entity&, const Delta_time)>;
  // <self ptr, dst, offset>
  using Draw_bg_cb = std::function<void (cr<Entity>, Image&, const Vec)>;
  using Draw_fg_cb = Draw_bg_cb;
  // <self ptr>
  using Kill_cb = std::function<void (Entity&)>;
  using Remove_cb = Kill_cb;

  inline explicit Entity_cbs(Entity& master): _master {master} {}
  #define MAKE_CB_DEF(FNAME, TYPE) \
    void FNAME(cr<TYPE> callback); \
    void FNAME(TYPE&& callback);
  MAKE_CB_DEF(add_update_cb, Update_cb)
  MAKE_CB_DEF(add_kill_cb, Kill_cb)
  MAKE_CB_DEF(add_draw_bg_cb, Draw_bg_cb)
  MAKE_CB_DEF(add_draw_fg_cb, Draw_fg_cb)
  MAKE_CB_DEF(add_remove_cb, Remove_cb)
  #undef MAKE_CB_DEF

protected:
  void process_kill_cbs();
  void process_update_cbs(Delta_time dt);
  void process_remove_cbs();
  void process_draw_fg_cbs(Image& dst, const Vec offset) const;
  void process_draw_bg_cbs(Image& dst, const Vec offset) const;

private:
  Vector<Update_cb> _update_cbs {}; // внешние колбэки на обработку апдейта
  Vector<Kill_cb> _kill_cbs {}; // внешние колбэки на обработку смерти
  Vector<Remove_cb> _remove_cbs {}; // колюэки при удалении из системы
  Vector<Draw_bg_cb> _draw_bg_cbs {}; // вызываются при отрисовке под объектом
  Vector<Draw_fg_cb> _draw_fg_cbs {}; // вызываются при отрисовке над объектом
}; // Entity_cbs
