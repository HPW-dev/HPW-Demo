#include "ability-entry.hpp"

struct Ability_entry::Impl {
  Player& _player;

  inline explicit Impl(Player& player): _player{player} {}

  inline void update(Delta_time dt) {
    // TODO
  }

  inline void draw_bg(Image& dst, const Vec offset) const {
    // TODO
  }

  inline void draw_fg(Image& dst, const Vec offset) const {
    // TODO
  }

  inline void clear() {
    // TODO
  }

  inline void give(Ability_id id, uint lvl) {
    // TODO
  }

  inline void give_or_upgrade(Ability_id id, uint lvl) {
    // TODO
  }

  inline uint level(Ability_id id) const {
    // TODO
    return {};
  }

  inline void remove(Ability_id id) {
    // TODO
  }

  inline void downgrade(Ability_id id, uint lvl) {
    // TODO
  }

  inline void upgrade(Ability_id id, uint lvl) {
    // TODO
  }

  inline bool exist(Ability_id id) const { 
    // TODO
    return false;
  }
}; // Impl

Ability_entry::Ability_entry(Player& player): impl{new_unique<Impl>(player)} {}
Ability_entry::~Ability_entry() {}
void Ability_entry::clear() { impl->clear(); }
void Ability_entry::update(Delta_time dt) { impl->update(dt); }
void Ability_entry::draw_bg(Image& dst, const Vec offset) const { impl->draw_bg(dst, offset); }
void Ability_entry::draw_fg(Image& dst, const Vec offset) const { impl->draw_fg(dst, offset); }
void Ability_entry::give(Ability_id id, uint lvl) { impl->give(id, lvl); }
void Ability_entry::give_or_upgrade(Ability_id id, uint lvl) { impl->give_or_upgrade(id, lvl); }
uint Ability_entry::level(Ability_id id) const { return impl->level(id); }
void Ability_entry::remove(Ability_id id) { impl->remove(id); }
void Ability_entry::downgrade(Ability_id id, uint lvl) { impl->downgrade(id, lvl); }
void Ability_entry::upgrade(Ability_id id, uint lvl) { impl->upgrade(id, lvl); }
bool Ability_entry::exist(Ability_id id) const { return impl->exist(id); }
