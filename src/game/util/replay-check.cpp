#include <cassert>
#include <iostream>
#include "replay-check.hpp"
#include "game/core/core.hpp"
#include "game/core/entities.hpp"
#include "game/entity/entity-manager.hpp"
#include "game/entity/entity-type.hpp"
#include "game/entity/player.hpp"
#include "game/entity/util/phys.hpp"
#include "util/math/num-types.hpp"
#include "util/math/vec-util.hpp"
#include "util/math/random.hpp"

uint chunk_id {0};

void phys_log(const Phys& phys) {
  std::clog << "pos: " << to_str(phys.get_pos()) << "\n";
  std::clog << "vel: " << to_str(phys.get_vel()) << "\n";
  std::clog << "acc: " << phys.get_accel() << "\n";
  std::clog << "spd: " << phys.get_speed() << "\n";
  std::clog << "deg: " << phys.get_deg() << "\n";
  std::clog << "frc: " << phys.get_force() << "\n";
}

void rnd_log() {
  std::clog << "random:\n";
  std::clog << "  seed: " << get_rnd_seed() << "\n";
  std::clog << "  rnd: " << rnd() << "\n";
  std::clog << "  rndu: " << rndu() << "\n";
  std::clog << "  rndr: " << rndr() << "\n";
  std::clog << "  rndb: " << +rndb() << "\n";
}

void entities_log() {
  std::clog << "entities:\n";

  cauto entities = hpw::entity_mgr->get_entities();
  if (entities.empty()) {
    std::clog << "no entities data\n";
    return;
  }

  cauto player = hpw::entity_mgr->get_player();
  for (cnauto entity: entities) {
    cont_if(entity.get() == player);

    assert(entity);
    std::clog << std::boolalpha << "status:\n";
    std::clog << std::boolalpha << "  live: " << entity->status.live << "\n";
    std::clog << std::boolalpha << "  killed: " << entity->status.killed << "\n";
    std::clog << std::boolalpha << "  collided: " << entity->status.collided << "\n";

    if (entity->status.collidable) {
      cauto collidable = dcast<Collidable*>(entity.get());
      assert(collidable);
      std::clog << "dmg: " << collidable->get_dmg() << "\n";
      std::clog << "hp: " << collidable->get_hp() << "\n";
    }

    phys_log(entity->phys);
  }
} // entities_log

void player_log() {
  std::clog << "player:\n";

  cauto player = hpw::entity_mgr->get_player();
  if (!player) {
    std::clog << "no player data\n";
    return;
  }

  std::clog << std::boolalpha << "status:\n";
  std::clog << std::boolalpha << "  live: " << player->status.live << "\n";
  std::clog << std::boolalpha << "  killed: " << player->status.killed << "\n";
  std::clog << std::boolalpha << "  collided: " << player->status.collided << "\n";

  std::clog << std::boolalpha << "ability:\n";
  std::clog << std::boolalpha << "  direct_shoot: " << player->ability.direct_shoot << "\n";
  std::clog << std::boolalpha << "  graze_en_regen: " << player->ability.graze_en_regen << "\n";
  std::clog << std::boolalpha << "  invisible: " << player->ability.invisible << "\n";
  std::clog << std::boolalpha << "  power_shoot: " << player->ability.power_shoot << "\n";
  std::clog << std::boolalpha << "  shield: " << player->ability.shield << "\n";
  std::clog << std::boolalpha << "  speedup_move: " << player->ability.speedup_move << "\n";
  std::clog << std::boolalpha << "  speedup_shoot: " << player->ability.speedup_shoot << "\n";
  std::clog << std::boolalpha << "  stillness_en_regen: " << player->ability.stillness_en_regen << "\n";
  
  std::clog << "energy: " << player->energy << "\n";
  std::clog << "hp: " << player->get_hp() << "\n";
  std::clog << "fuel: " << player->m_fuel << "\n";

  phys_log(player->phys);
} // player_log

void replay_stable_log() {
  std::clog << "replay_stable_log(): chunk: " << chunk_id++ << "\n";

  std::clog << "game_updates_safe: " << hpw::game_updates_safe << "\n";
  rnd_log();
  entities_log();
  player_log();

  std::clog << "\n";
} // replay_stable_log
