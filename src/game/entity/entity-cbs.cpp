#include "entity-cbs.hpp"
#include "util/math/vec.hpp"

#define MAKE_CB_IMPL(FNAME, TYPE, VAR) \
  void Entity_cbs::FNAME(cr<TYPE> callback) { \
    return_if(!callback); \
    VAR.push_back(callback); \
  } \
  \
  void Entity_cbs::FNAME(TYPE&& callback) { \
    return_if(!callback); \
    VAR.emplace_back(std::move(callback)); \
  }
MAKE_CB_IMPL(add_kill_cb, Kill_cb, _kill_cbs)
MAKE_CB_IMPL(add_update_cb, Update_cb, _update_cbs)
MAKE_CB_IMPL(add_remove_cb, Remove_cb, _remove_cbs)
MAKE_CB_IMPL(add_draw_bg_cb, Draw_bg_cb, _draw_bg_cbs)
MAKE_CB_IMPL(add_draw_fg_cb, Draw_fg_cb, _draw_fg_cbs)
#undef MAKE_CB_IMPL

void Entity_cbs::process_remove_cbs() {
  for (rauto cb: _remove_cbs)
    cb(_master);
}

void Entity_cbs::process_draw_fg_cbs(Image& dst, const Vec offset) const {
  for (crauto cb: _draw_fg_cbs)
    cb(_master, dst, offset);
}

void Entity_cbs::process_draw_bg_cbs(Image& dst, const Vec offset) const {
  for (crauto cb: _draw_bg_cbs)
    cb(_master, dst, offset);
}

void Entity_cbs::process_update_cbs(Delta_time dt) {
  for (rauto cb: _update_cbs)
    cb(_master, dt);
}

void Entity_cbs::process_kill_cbs() {
  for (rauto cb: _kill_cbs)
    cb(_master);
}
