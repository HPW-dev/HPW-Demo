#include "pch.hpp"
#include "collidable-info.hpp"
#include "game/core/entities.hpp"
#include "game/entity/collidable.hpp"
#include "game/entity/util/entity-util.hpp"

void Collidable_info::load(cr<nlohmann::json> node) {
  hp = node.value("hp", int{0});
  dmg = node.value("dmg", int{0});
  explosion_name = node.value("explosion", Str{});
  ignore_enemy = node.value("ignore_enemy", false);
  ignore_bullet = node.value("ignore_bullet", false);
  ignore_self_type = node.value("ignore_self_type", false);
  ignore_master = node.value("ignore_master", true);
  ignore_player = node.value("ignore_player", false);
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
    dst.add_kill_cb([_expl_name=explosion_name](Entity& ent) {
      assert(hpw::entity_mgr);
      hpw::entity_mgr->make(&ent, _expl_name, ent.phys.get_pos());
    });
}
