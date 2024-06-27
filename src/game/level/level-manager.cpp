#include <cassert>
#include "level.hpp"
#include "game/core/fonts.hpp"
#include "game/core/core.hpp"
#include "game/core/levels.hpp"
#include "game/core/debug.hpp"
#include "game/core/scenes.hpp"
#include "game/core/entities.hpp"
#include "game/entity/util/phys.hpp"
#include "game/entity/player/player.hpp"
#include "game/util/game-util.hpp"
#include "graphic/image/image.hpp"
#include "util/log.hpp"
#include "util/safecall.hpp"
#include "util/math/vec.hpp"

Level_mgr::Level_mgr(CN<Makers> _makers)
: m_makers(_makers) {
  iflog(m_makers.empty(), "Генераторы уровней не заданы\n");
}

void Level_mgr::update(const Vec vel, Delta_time dt) {
  if (m_level) {
    m_level->update(vel, dt);
    if (m_level->m_complete)
      m_level = {};
  } 
  // смена уровня
  if (!m_level) {
    hpw::entity_mgr->clear();
    accept_maker();
    
    if (cauto _level_name = level_name(); !_level_name.empty())
      detailed_log("выбран уровень: \"" << _level_name << "\"\n");
  } // if !m_level
} // update

void Level_mgr::draw(Image& dst) const {
  return_if(!m_visible);

  if (m_level) {
    // показать уровень, иначе залить фон
    if (graphic::draw_level)
      m_level->draw(dst);
    else
      dst.fill({}); // TODO для красивого эффекта это можно вырубить
  } else {
    dst.fill(Pal8::black);
    graphic::font->draw(dst, get_screen_center(), U"NO LEVEL");
  }
}

void Level_mgr::draw_upper_layer(Image& dst) const {
  // показать уровень, иначе залить фон
  if (graphic::draw_level && m_level)
    m_level->draw_upper_layer(dst);
}

void Level_mgr::accept_maker() {
  if (m_makers.empty()) {
    end_of_levels = true;
  } else {
    // взять следующий уровень и убрать его из списка на создание
    auto maker = m_makers.front();
    m_level = maker();
    m_makers.pop_front();
  }
}

void Level_mgr::finalize_level() {
  auto player = hpw::entity_mgr->get_player();
  assert(player);
  set_player_prev_pos(player->phys.get_pos());
  
  if (m_level)
    m_level->m_complete = true;
}

Vec Level_mgr::player_prev_lvl_pos() const
  { return m_player_pos_from_prev_level; }

void Level_mgr::set_player_prev_pos(const Vec pos)
  { m_player_pos_from_prev_level = pos; }

Str Level_mgr::level_name() const {
  if (m_level)
    return m_level->level_name();
  detailed_log("empty m_level\n");
  return {};
}

void Level_mgr::set_visible(bool mode) { m_visible = mode; }

void Level_mgr::set(CN<Level_mgr::Maker> maker) { 
  assert(maker);
  if (hpw::entity_mgr)
    hpw::entity_mgr->clear();
  m_level = maker();
}
