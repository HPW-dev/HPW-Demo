#include "ability-entry.hpp"

struct Ability_entry::Impl {
  Player& _player;

  inline explicit Impl(Player& player): _player{player} {}
  inline void update(Delta_time dt) {}
  inline void draw_bg(Image& dst, const Vec offset) const {}
  inline void draw_fg(Image& dst, const Vec offset) const {}
  inline void clear() {}
};

Ability_entry::Ability_entry(Player& player): impl{new_unique<Impl>(player)} {}
Ability_entry::~Ability_entry() {}
void Ability_entry::clear() { impl->clear(); }
void Ability_entry::update(Delta_time dt) { impl->update(dt); }
void Ability_entry::draw_bg(Image& dst, const Vec offset) const { impl->draw_bg(dst, offset); }
void Ability_entry::draw_fg(Image& dst, const Vec offset) const { impl->draw_fg(dst, offset); }
