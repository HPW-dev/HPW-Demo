#pragma once
#include <functional>
#include <optional>
#include "sound/audio.hpp"
#include "util/str.hpp"
#include "util/macro.hpp"
#include "util/math/num-types.hpp"
#include "util/math/vec.hpp"
#include "util/math/rect.hpp"
#include "util/math/vec.hpp"

class Entity;
class Particle;
class Collidable;
class Phys;
struct Vec3;

// рекомендуемый колижн ресолвер для шмап мода
void set_default_collider();
// генерирует UID
Uid get_entity_uid();
// для синхронизации в реплеях
void clear_entity_uid();
// безопасно добавить анимацию к объекту
void add_anim(Entity& dst, cr<Str> anim_name);
// каст частицу (с проверкой)
Particle* to_particle(Entity& src);
// каст сталкиваемый объект (с проверкой)
Collidable* to_collidable(Entity& src);
// для отскока от экрана
void bounce_off_screen(Entity& entity, Delta_time dt);
// проверить что a и b могут сталкиваться вместе
bool cld_flag_compat(cr<Entity> a, cr<Entity> b);
// Убить объект, если создатель объекта мёртв
void kill_if_master_death(Entity& entity, Delta_time dt);
// на сколько надо перекрутиться, чтобы моделька смотрела на позицию target
real need_deg_to_target(cr<Entity> self, cr<Entity> target);
// надо ли поворачивать вправо, чтобы моделька смотрела на target?
bool need_rotate_right(cr<Entity> self, cr<Entity> target);
// на сколько надо перекрутиться, чтобы моделька смотрела на позицию target
real need_deg_to_target(cr<Entity> self, const Vec target);
// надо ли поворачивать вправо, чтобы моделька смотрела на target?
bool need_rotate_right(cr<Entity> self, const Vec target);
// угол поворота к цели
real deg_to_target(const Vec self, const Vec target);
/** найти точку упреждения движения
* @param self позиция стреляющего объекта
* @param target движущаяся цель, в которую стреляют */
Vec predict(cr<Phys> self, cr<Phys> target);
// проверяет что объект в пределах экрана
bool bound_check_for_collisions(cr<Collidable> other);
// кновертирует ускорение игры в звуковое ускорение
Vec3 to_sound_vel(const Vec src);
// кновертирует корды игры в звуковые корды
Vec3 to_sound_pos(const Vec src);
// хитбокс игрока в виде прямоугольника
[[nodiscard]] std::optional<Rect> get_player_rect();

class Kill_by_timeout final {
  Delta_time m_timeout {};
public:
  explicit Kill_by_timeout(const Delta_time timeout=10);
  void operator()(Entity& entity, Delta_time dt);
};

// замедляет анимаю со временем
class Anim_speed_slowdown final {
  Delta_time m_slowndown {}; // скорость замедления
public:
  explicit Anim_speed_slowdown(const Delta_time slowndown);
  void operator()(Entity& entity, Delta_time dt);
};

// зависимость скорости анимации от скорости движения объекта
class Anim_speed_addiction final {
  Delta_time m_target_speed {};
  Delta_time m_min_ratio {};
  Delta_time m_max_ratio {};
public:
  /* @param target_speed лимит скорости, после которого идёт ускорение анимации
     @param min_ratio на сколько анимацию можно замедлить
     @param max_ratio на сколько анимацию можно ускорить */
  explicit Anim_speed_addiction(Delta_time target_speed,
    Delta_time min_ratio, Delta_time max_ratio);
  void operator()(Entity& entity, Delta_time dt);
};

// зависимость скорости вращения (анимации) от скорости движения объекта
class Rotate_speed_addiction final {
  Delta_time m_target_speed {};
  Delta_time m_min_ratio {};
  Delta_time m_max_ratio {};
  Delta_time m_speed_scale {};
  bool m_rot_right {};
public:
  /* @param target_speed лимит скорости, после которого ускоряется вращение
     @param min_ratio на сколько анимацию можно замедлить
     @param max_ratio на сколько анимацию можно ускорить
     @param speed_scale усиливает скорость вращения
     @param rot_right вращаться влево */
  explicit Rotate_speed_addiction(Delta_time target_speed,
    Delta_time min_ratio, Delta_time max_ratio, Delta_time speed_scale=1, bool rot_right=true);
  void operator()(Entity& entity, Delta_time dt);
};

// Не делать объект невидимым, пока не закончится таймер
class Timed_visible final {
  Delta_time m_timeout {};
public:
  explicit Timed_visible(const Delta_time timeout);
  void operator()(Entity& entity, Delta_time dt);
};

// умереть с задержкой, если объект родитель умер
class Timed_kill_if_master_death final {
  Delta_time m_delay {};
  bool m_master_death {};
public:
  explicit Timed_kill_if_master_death(const Delta_time delay);
  void operator()(Entity& entity, Delta_time dt);
};

// ограничитель позиции игрока в пределах экрана
struct Bound_off_screen final {
  Vec screen_lu {}; // ограничение слева сверху
  Vec screen_rd {}; // ограничение справа снизу

  explicit Bound_off_screen(cr<Entity> src);
  void operator()(Entity& dst, const Delta_time dt);
};

// поворачивает объект в стророну цели
class Rotate_to_target final {
public:
  using Target_getter = std::function<Vec ()>;
  explicit Rotate_to_target(cr<Target_getter> target_getter, const real rotate_speed);
  void operator()(Entity& self, const Delta_time dt);

private:
  Target_getter _target_getter {};
  real _rotate_speed {};
};

// Приклеивает звук к объекту
class Sound_attached final {
  Entity& _entity;
  Audio_id _audio_id {};

public:
  explicit Sound_attached(Entity& ent, cr<Str> sound_name, bool repeat=false, real gain=1.0);
  void operator()(Entity& ent, const Delta_time dt);
};
