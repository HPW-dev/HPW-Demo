#include <cassert>
#include "collidable-info.hpp"
#include "game/core/entities.hpp"
#include "game/entity/collidable.hpp"
#include "game/entity/util/entity-util.hpp"
#include "util/file/yaml.hpp"

void Collidable_info::load(cr<Yaml> node) {
  hp = node.get_int("hp");
  dmg = node.get_int("dmg");
  explosion_name = node.get_str("explosion");
  ignore_enemy = node.get_bool("ignore_enemy");
  ignore_bullet = node.get_bool("ignore_bullet");
  ignore_self_type = node.get_bool("ignore_self_type");
  ignore_master = node.get_bool("ignore_master", true);
  ignore_player = node.get_bool("ignore_player");
}

void Collidable_info::accept(Collidable& dst) {
  dst.set_dmg(dmg);
  dst.set_hp(hp);
  dst.status.ignore_enemy = ignore_enemy;
  dst.status.ignore_bullet = ignore_bullet;
  dst.status.ignore_self_type = ignore_self_type;
  dst.status.ignore_master = ignore_master;
  dst.status.ignore_player = ignore_player;

  // добавить колбэк на создание взрыва при смерти
  if (!explosion_name.empty())
    dst.add_kill_cb([=](Entity& ent) {
      assert(hpw::entity_mgr);
      hpw::entity_mgr->make(&ent, explosion_name, ent.phys.get_pos());
    });
}
