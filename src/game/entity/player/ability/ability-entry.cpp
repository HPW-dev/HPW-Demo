#include <cassert>
#include <unordered_map>
#include "ability-entry.hpp"
#include "ability-util.hpp"
#include "ability.hpp"

struct Ability_entry::Impl {
  using Abilities = std::unordered_map<Ability_id, Unique<Ability>>;
  Abilities _abilities {};
  Player& _player;

  inline explicit Impl(Player& player): _player{player} {}

  inline void update(Delta_time dt) {
    kill_if_zero_lvl();

    for (crauto [_, ability]: _abilities) {
      assert(ability->id() != Ability_id::error);
      ability->update(dt);
    }
  }

  inline void kill_if_zero_lvl() {
    std::erase_if(_abilities, [](cr<decltype(_abilities)::value_type> id_and_name) {
      return id_and_name.second->level() == 0;
    });
  }

  inline void draw_bg(Image& dst, const Vec offset) const {
    for (crauto [_, ability]: _abilities)
      ability->draw_bg(dst, offset);
  }

  inline void draw_fg(Image& dst, const Vec offset) const {
    for (crauto [_, ability]: _abilities)
      ability->draw_fg(dst, offset);
  }

  inline void clear_abilities() { _abilities.clear(); }

  inline void give(Ability_id id, uint lvl) {
    _abilities[id] = make_ability(id, lvl);
  }

  inline void give_or_upgrade(Ability_id id, uint lvl) {
    if (exist(id))
      upgrade(id, lvl);
    else
      give(id, lvl);
  }

  inline uint level(Ability_id id) const {
    try {
      rauto ability = _abilities.at(id);
      return_if (ability, ability->level());
    } catch (...) {}

    return 0;
  }

  inline void remove(Ability_id id) { _abilities.erase(id); }

  inline void downgrade(Ability_id id, uint lvl) {
    try {
      rauto ability = _abilities.at(id);
      return_if (!ability);
      cfor(i, lvl)
        ability->on_downgrade();
    } catch (...) {}
  }

  inline void upgrade(Ability_id id, uint lvl) {
    try {
      rauto ability = _abilities.at(id);
      return_if (!ability);
      cfor(i, lvl)
        ability->on_upgrade();
    } catch (...) {}
  }

  inline bool exist(Ability_id id) const { 
    return _abilities.find(id) != _abilities.end();
  }

  inline Vector<cp<Ability>> abilities() const {
    Vector<cp<Ability>> ret;
    for (crauto [_, ability]: _abilities)
      ret.push_back(ability.get());
    return ret;
  }
}; // Impl

Ability_entry::Ability_entry(Player& player): impl{new_unique<Impl>(player)} {}
Ability_entry::~Ability_entry() {}
void Ability_entry::clear_abilities() { impl->clear_abilities(); }
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
Vector<cp<Ability>> Ability_entry::abilities() const { return impl->abilities(); }
