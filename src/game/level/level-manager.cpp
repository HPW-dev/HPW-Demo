#include <cassert>
#include "level-manager.hpp"
#include "level.hpp"
#include "game/core/core.hpp"
#include "game/core/debug.hpp"
#include "game/core/scenes.hpp"
#include "game/core/entitys.hpp"
#include "game/entity/entity-manager.hpp"
#include "game/entity/util/phys.hpp"
#include "game/entity/player.hpp"
#include "graphic/image/image.hpp"
#include "util/log.hpp"
#include "util/math/vec.hpp"

Level_mgr::Level_mgr(CN<Makers> _makers)
: makers(_makers) {
  detailed_iflog(makers.empty(), "Level_mgr.c-tor makers is empty\n");
}

void Level_mgr::update(const Vec vel, double dt) {
  assert(dt == hpw::target_update_time);

  if (level) {
    level->update(vel, dt);
    if (level->complete)
      level = {};
  } else { // смена уровня
    hpw::entity_mgr->clear();
    accept_maker();
  }
} // update

void Level_mgr::draw(Image& dst) const {
  // показать уровень, иначе залить фон
  if (graphic::draw_level && level)
    level->draw(dst);
  else
    dst.fill({}); // TODO для красивого эффекта это можно вырубить
}

void Level_mgr::draw_upper_layer(Image& dst) const {
  // показать уровень, иначе залить фон
  if (graphic::draw_level && level)
    level->draw_upper_layer(dst);
}

void Level_mgr::accept_maker() {
  if (makers.empty()) {
    end_of_levels = true;
  } else {
    // взять следующий уровень и убрать его из списка на создание
    auto maker = makers.front();
    level = maker();
    makers.pop_front();
  }
}

void Level_mgr::finalize_level() {
  auto player = hpw::entity_mgr->get_player();
  assert(player);
  set_player_pos_from_prev_level(player->phys.get_pos());
  
  if (level)
    level->complete = true;
}

Vec Level_mgr::get_player_pos_from_prev_level() const
  { return m_player_pos_from_prev_level; }

void Level_mgr::set_player_pos_from_prev_level(const Vec pos)
  { m_player_pos_from_prev_level = pos; }
