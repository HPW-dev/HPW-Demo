#include "ability-dummy.hpp"
#include <format>
#include "util/log.hpp"

Ability_dummy:: Ability_dummy() {
  log_info << "dummy c-tor";
}

Ability_dummy::~Ability_dummy() {
  log_info << "dummy d-tor";
}

Ability_id Ability_dummy::id() const {
  return Ability_id::dummy;
}

void Ability_dummy::draw_bg(Image& dst, const Vec offset) {
  Ability::draw_bg(dst, offset);
}

void Ability_dummy::draw_fg(Image& dst, const Vec offset) {
  Ability::draw_fg(dst, offset);
}

void Ability_dummy::update(Delta_time dt) {
  Ability::update(dt);
}

void Ability_dummy::on_upgrade() {
  Ability::on_upgrade();
  log_info << std::format("dummy upgraded. New lvl = {}", level());
}

void Ability_dummy::on_downgrade() {
  Ability::on_downgrade();
  log_info << std::format("dummy downgraded. New lvl = {}", level());
}
