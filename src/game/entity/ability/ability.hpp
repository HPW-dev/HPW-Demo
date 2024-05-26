#pragma once
#include "util/str.hpp"
#include "util/unicode.hpp"
#include "util/math/vec.hpp"

class Player;
class Image;
class Sprite;

// базовый класс для способностей игрока
class Ability {
public:
  inline explicit Ability(const std::size_t _type_id): m_type_id {_type_id} {}
  virtual ~Ability() = default;

  inline std::size_t type_id() const { return m_type_id; }
  inline virtual void update(Player& player, const double dt) = 0;
  inline virtual void draw(Image& dst, const Vec offset) const = 0;
  inline virtual void power_up() = 0;
  // название способности
  inline virtual utf32 name() const = 0;
  // описание способности
  inline virtual utf32 desc() const = 0;
  // значок со способностью (берётся из банка)
  inline virtual CP<Sprite> icon() const = 0;

private:
  nocopy(Ability);
  std::size_t m_type_id {}; // идентификатор типа для сравенения
};
