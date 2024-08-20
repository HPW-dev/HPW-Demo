#include "ability-dummy.hpp"
#include "util/log.hpp"

Ability_dummy:: Ability_dummy() {
  hpw_log("dummy c-tor\n");
}

Ability_dummy::~Ability_dummy() {
  hpw_log("dummy d-tor\n");
}

Ability_id Ability_dummy::id() const {
  return Ability_id::dummy;
}

void Ability_dummy::draw_bg(Image& dst, const Vec offset) {
  Ability::draw_bg(dst, offset);
  // hpw_log("dummy draw BG\n");
}

void Ability_dummy::draw_fg(Image& dst, const Vec offset) {
  Ability::draw_fg(dst, offset);
  // hpw_log("dummy draw FG\n");
}

void Ability_dummy::update(Delta_time dt) {
  Ability::update(dt);
  // hpw_log("dummy update\n");
}

void Ability_dummy::on_upgrade() {
  Ability::on_upgrade();
  hpw_log("dummy upgraded. New lvl = " << level() << "\n");
}

void Ability_dummy::on_downgrade() {
  Ability::on_downgrade();
  hpw_log("dummy downgraded. New lvl = " << level() << "\n");
}
