#pragma once
#include "entity.hpp"
#include "util/str.hpp"

// Всё что способно сталкиваться и дохнуть от дамага
class Collidable: public Entity {
  nocopy(Collidable);
  hp_t m_hp {}; // жизни (можно сносить в минус)
  hp_t m_dmg {}; // урон от столкновения с объектом
  Str m_explosion_name {}; // имя анимации взрыва

  void draw_hitbox(Image& dst, const Vec offset) const;

public:
  void draw(Image& dst, const Vec offset) const override;
  void update(const Delta_time dt) override;
  bool is_collided_with(CN<Collidable> other) const;
  CP<Hitbox> get_hitbox() const override;
  void kill() override;

  // нанесение урона
  virtual void sub_hp(hp_t incoming_dmg);
  inline void set_hp(hp_t val) { m_hp = val; }
  inline void set_dmg(hp_t val) { m_dmg = val; }
  inline hp_t get_hp() const { return m_hp; }
  inline hp_t get_dmg() const { return m_dmg; }
  // имя взрыва
  inline CN<Str> get_explosion_name() const { return m_explosion_name; }
  inline void set_explosion_name(CN<Str> name) { m_explosion_name = name; }

  Collidable();
  explicit Collidable(Entity_type new_type);
  ~Collidable() = default;
}; // Collidable
