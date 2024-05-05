#pragma once
#include "util/str.hpp"
#include "util/macro.hpp"
#include "util/math/num-types.hpp"
#include "util/math/vec.hpp"

class Entity;
class Particle;
class Collidable;
class Phys;

// генерирует UID
Uid get_entity_uid();
// для синхронизации в реплеях
void clear_entity_uid();
// безопасно добавить анимацию к объекту
void add_anim(Entity& dst, CN<Str> anim_name);
// каст частицу (с проверкой)
Particle* to_particle(Entity& src);
// каст сталкиваемый объект (с проверкой)
Collidable* to_collidable(Entity& src);
// для отскока от экрана
void bounce_off_screen(Entity& entity, double dt);
// проверить что a и b могут сталкиваться вместе
bool cld_flag_compat(CN<Entity> a, CN<Entity> b);

class Kill_by_timeout final {
  double m_timeout {};
public:
  explicit Kill_by_timeout(double timeout=10);
  void operator()(Entity& entity, double dt);
};

// замедляет анимаю со временем
class Anim_speed_slowdown final {
  double m_slowndown {}; // скорость замедления
public:
  explicit Anim_speed_slowdown(double slowndown);
  void operator()(Entity& entity, double dt);
};

// зависимость скорости анимации от скорости движения объекта
class Anim_speed_addiction final {
  double m_target_speed {};
  double m_min_ratio {};
  double m_max_ratio {};
public:
  /* @param target_speed лимит скорости, после которого идёт ускорение анимации
     @param min_ratio на сколько анимацию можно замедлить
     @param max_ratio на сколько анимацию можно ускорить */
  explicit Anim_speed_addiction(double target_speed,
    double min_ratio, double max_ratio);
  void operator()(Entity& entity, double dt);
};

// зависимость скорости вращения (анимации) от скорости движения объекта
class Rotate_speed_addiction final {
  double m_target_speed {};
  double m_min_ratio {};
  double m_max_ratio {};
  double m_speed_scale {};
  bool m_rot_right {};
public:
  /* @param target_speed лимит скорости, после которого ускоряется вращение
     @param min_ratio на сколько анимацию можно замедлить
     @param max_ratio на сколько анимацию можно ускорить
     @param speed_scale усиливает скорость вращения
     @param rot_right вращаться влево */
  explicit Rotate_speed_addiction(double target_speed,
    double min_ratio, double max_ratio, double speed_scale=1, bool rot_right=true);
  void operator()(Entity& entity, double dt);
};

// на сколько надо перекрутиться, чтобы моделька смотрела на позицию target
real need_deg_to_target(CN<Entity> self, CN<Entity> target);
// надо ли поворачивать вправо, чтобы моделька смотрела на target?
bool need_rotate_right(CN<Entity> self, CN<Entity> target);
// на сколько надо перекрутиться, чтобы моделька смотрела на позицию target
real need_deg_to_target(CN<Entity> self, const Vec target);
// надо ли поворачивать вправо, чтобы моделька смотрела на target?
bool need_rotate_right(CN<Entity> self, const Vec target);
// угол поворота к цели
real deg_to_target(CN<Entity> self, CN<Entity> target);
// угол поворота к цели
real deg_to_target(CN<Entity> self, const Vec target);

/** найти точку упреждения движения
* @param self позиция стреляющего объекта
* @param target движущаяся цель, в которую стреляют */
Vec predict(CN<Entity> self, CN<Entity> target, double dt);
