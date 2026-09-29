#include "pch.hpp"
#include "collidable-info.hpp"
#include "game/core/entities.hpp"
#include "game/entity/collidable.hpp"
#include "game/entity/util/entity-util.hpp"

void Collidable_info::load(cr<nlohmann::json> node) {
  hp = node["hp"].get<int>();
  dmg = node["dmg"].get<int>();
  explosion_name = node["explosion"].get<Str>();
  ignore_enemy = node["ignore_enemy"].get<bool>();
  ignore_bullet = node["ignore_bullet"].get<bool>();
  ignore_self_type = node["ignore_self_type"].get<bool>();
  ignore_master = node.value<bool>("ignore_master", true);
  ignore_player = node["ignore_player"].get<bool>();
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
