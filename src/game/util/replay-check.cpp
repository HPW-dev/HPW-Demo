#ifdef STABLE_REPLAY
#include <cassert>
#include <sstrea,>
#include "replay-check.hpp"
#include "game/core/core.hpp"
#include "game/core/entities.hpp"
#include "game/util/keybits.hpp"
#include "game/entity/entity-type.hpp"
#include "game/entity/player/player.hpp"
#include "game/entity/util/phys.hpp"
#include "util/math/num-types.hpp"
#include "util/math/vec-util.hpp"
#include "util/math/random.hpp"

uint chunk_id {0}; // для визуального разграничения

void phys_log(const Phys& phys) {
  std::stringstream ss;
  ss << "pos: " << to_str(phys.get_pos()) << "\n";
  ss << "vel: " << to_str(phys.get_vel()) << "\n";
  ss << "acc: " << phys.get_accel() << "\n";
  ss << "spd: " << phys.get_speed() << "\n";
  ss << "deg: " << phys.get_deg() << "\n";
  ss << "frc: " << phys.get_force() << "\n";
  hpw_log(ss.str());
}

void rnd_log() {
  std::stringstream ss;
  ss << "random:\n";
  ss << "  seed: " << get_rnd_seed() << "\n";
  ss << "  rnd: " << rnd() << "\n";
  ss << "  rndu: " << rndu() << "\n";
  ss << "  rndr: " << rndr() << "\n";
  ss << "  rndb: " << +rndb() << "\n";
  hpw_log(ss.str());
}

void entities_log() {
  hpw_log("entities:\n");

  cauto entities = hpw::entity_mgr->get_entities();
  if (entities.empty()) {
    hpw_log("no entities data\n");
    return;
  }

  cauto player = hpw::entity_mgr->get_player();
  for (crauto entity: entities) {
    cont_if(entity.get() == player);

    assert(entity);
    std::stringstream ss;
    ss << std::boolalpha << "status:\n";
    ss << std::boolalpha << "  live: " << entity->status.live << "\n";
    ss << std::boolalpha << "  killed: " << entity->status.killme << "\n";
    ss << std::boolalpha << "  collided: " << entity->status.collided << "\n";

    if (entity->status.collidable) {
      cauto collidable = dcast<Collidable*>(entity.get());
      assert(collidable);
      ss << "dmg: " << collidable->get_dmg() << "\n";
      ss << "hp: " << collidable->get_hp() << "\n";
    }

    hpw_log(ss.str());
    phys_log(entity->phys);
  }
} // entities_log

void player_log() {
  std::stringstream ss;
  ss << "player:\n";

  cauto player = hpw::entity_mgr->get_player();
  if (!player) {
    ss << "no player data\n";
    return;
  }

  ss << std::boolalpha << "status:\n";
  ss << std::boolalpha << "  live: " << player->status.live << "\n";
  ss << std::boolalpha << "  killed: " << player->status.killme << "\n";
  ss << std::boolalpha << "  collided: " << player->status.collided << "\n";

  /* TODO
  ss << std::boolalpha << "ability:\n";
  ss << std::boolalpha << "  direct_shoot: " << player->check_capability(Ability.direct_shoot) << "\n";
  ss << std::boolalpha << "  graze_en_regen: " << player->check_capability(Ability.graze_en_regen) << "\n";
  ss << std::boolalpha << "  invisible: " << player->check_capability(Ability.invisible) << "\n";
  ss << std::boolalpha << "  power_shoot: " << player->check_capability(Ability.power_shoot) << "\n";
  ss << std::boolalpha << "  shield: " << player->check_capability(Ability.shield) << "\n";
  ss << std::boolalpha << "  speedup_move: " << player->check_capability(Ability.speedup_move) << "\n";
  ss << std::boolalpha << "  speedup_shoot: " << player->check_capability(Ability.speedup_shoot) << "\n";
  ss << std::boolalpha << "  stillness_en_regen: " << player->check_capability(Ability.stillness_en_regen) << "\n";
  ss << std::boolalpha << "  home: " << player->check_capability(Ability.home) << "\n";
  ss << std::boolalpha << "  fullscreen_shoot: " << player->check_capability(Ability.fullscreen_shoot) << "\n";
  */
  
  ss << "energy: " << player->energy << "\n";
  ss << "hp: " << player->get_hp() << "\n";
  ss << "fuel: " << player->m_fuel << "\n";

  hpw_log(ss.str());
  phys_log(player->phys);
} // player_log

void input_log() {
  std::stringstream ss;
  ss << "input (UDLRSMBFE): ";
  ss << (is_pressed(hpw::keycode::up) ? 1 : 0);
  ss << (is_pressed(hpw::keycode::down) ? 1 : 0);
  ss << (is_pressed(hpw::keycode::left) ? 1 : 0);
  ss << (is_pressed(hpw::keycode::right) ? 1 : 0);
  ss << (is_pressed(hpw::keycode::shoot) ? 1 : 0);
  ss << (is_pressed(hpw::keycode::mode) ? 1 : 0);
  ss << (is_pressed(hpw::keycode::bomb) ? 1 : 0);
  ss << (is_pressed(hpw::keycode::focus) ? 1 : 0);
  ss << (is_pressed(hpw::keycode::enable) ? 1 : 0);
  ss << "\n";
  hpw_log(ss.str());
}

void replay_stable_log() {
  std::stringstream ss;
  ss << "replay_stable_log(): chunk: " << chunk_id++ << "\n";

  ss << "game_ticks: " << hpw::game_ticks << "\n";
  rnd_log();
  entities_log();
  player_log();
  input_log();

  ss << "\n";
  hpw_log(ss.str());
} // replay_stable_log

#endif // STABLE_REPLAY
