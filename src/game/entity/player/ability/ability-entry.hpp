#pragma once
#include "ability-id.hpp"
#include "util/mem-types.hpp"
#include "util/math/num-types.hpp"
#include "util/math/vec.hpp"

class Player;
class Image;

// от этого надо наследоваться, чтобы получить способности
class Ability_entry {
  struct Impl;
  Unique<Impl> impl {};

protected:
  void update(Delta_time dt);
  void draw_bg(Image& dst, const Vec offset) const;
  void draw_fg(Image& dst, const Vec offset) const;

public:
  explicit Ability_entry(Player& player);
  ~Ability_entry();
  void clear_abilities(); // убрать все способности
  void give(Ability_id id, uint lvl=1);
  // добавит способность, но если она уже есть, повысит её уровень
  void give_or_upgrade(Ability_id id, uint lvl=1);
  // узнать уровень способности. Если её нет, то 0
  uint level(Ability_id id) const;
  void remove(Ability_id id);
  void downgrade(Ability_id id, uint lvl=1);
  void upgrade(Ability_id id, uint lvl=1);
  // проверить что способность есть
  bool exist(Ability_id id) const;
};
