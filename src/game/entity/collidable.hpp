#pragma once
#include <unordered_set>
#include "entity.hpp"
#include "util/str.hpp"
#include "util/omp-mutex.hpp"

// Всё что способно сталкиваться и дохнуть от дамага
class Collidable: public Entity {
  nocopy(Collidable);
  // список всех тек, кто столкнулся с объектом
  std::unordered_set<Collidable*> m_collided {};
  hp_t m_hp {}; // жизни (можно сносить в минус)
  hp_t m_dmg {}; // урон от столкновения с объектом
  Str m_explosion_name {}; // имя анимации взрыва
  mutable omp::mutex m_mutex {};
  mutable real m_old_deg {-999}; // для кэширования хитбокса
  mutable cp<Anim> m_old_anim {}; // для кэширования хитбокса
  mutable cp<Hitbox> m_old_hitbox {}; // кэшированный хитбокс

  // <self, other>
  using Collidion_cb = std::function<void (Collidable&, Collidable&)>;
  Vector<Collidion_cb> _collidion_cbs {}; // действия над объектами при столкновении

  void draw_hitbox(Image& dst, const Vec offset) const;
  // обработка смерти от потери хп. Ret true если умерли
  bool kill_by_damage();
  // нанести урон от всех, кто столкнулся в объект
  void process_damage();
  void process_collision_cbs();

public:
  void draw(Image& dst, const Vec offset) const override;
  void update(const Delta_time dt) override;
  cp<Hitbox> get_hitbox() const override;
  void add_collision_cb(cr<Collidion_cb> cb);
  void add_collision_cb(Collidion_cb&& cb);
  void process_kill() override;

  // нанесение урона
  virtual void sub_hp(hp_t incoming_dmg);
  inline void set_hp(hp_t val) { m_hp = val; }
  inline void set_dmg(hp_t val) { m_dmg = val; }
  inline hp_t get_hp() const { return m_hp; }
  inline hp_t get_dmg() const { return m_dmg; }
  // имя взрыва
  inline cr<Str> get_explosion_name() const { return m_explosion_name; }
  inline void set_explosion_name(cr<Str> name) { m_explosion_name = name; }
  // проверить что с таким объетом уже было столкновение
  bool collided_with(Collidable* other) const;
  // обработать столкновение с другим объектом. True - если было
  bool resolve_collision(Collidable& other);
  // проверяет что столкновение возможно
  bool collision_possible(Collidable& other) const;
  // выполняет действия при столкновении двух объектов
  void collide_with(Collidable& other);
  // определяет, что столкновение хитбоксов состоялось ранее
  bool is_collided_with(Collidable* other) const;
  // проверить столкновения хитбоксов
  bool hitbox_test(cr<Collidable> other) const;

  Collidable();
  ~Collidable() = default;
  explicit Collidable(Entity_type new_type);
}; // Collidable
