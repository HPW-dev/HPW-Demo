#include <cassert>
#include "level.hpp"
#include "level-mgr.hpp"
#include "game/core/fonts.hpp"
#include "game/core/core.hpp"
#include "game/core/levels.hpp"
#include "game/core/debug.hpp"
#include "game/core/tasks.hpp"
#include "game/core/scenes.hpp"
#include "game/core/entities.hpp"
#include "game/entity/util/phys.hpp"
#include "game/entity/player/player.hpp"
#include "game/util/game-util.hpp"
#include "graphic/image/image.hpp"
#include "util/log.hpp"
#include "util/safecall.hpp"
#include "util/math/vec.hpp"

Level_mgr::Level_mgr(cr<Makers> _makers)
: m_makers(_makers) {
  if (m_makers.empty())
    hpw_log("Генераторы уровней не заданы\n");
}

void Level_mgr::update(const Vec vel, Delta_time dt) {
  level_order_update();
  level_update(vel, dt);
}

void Level_mgr::level_update(const Vec vel, Delta_time dt) {
  return_if (!m_level);

  m_level->update(vel, dt);
  if (m_level->m_complete)
    on_end_level();
}

void Level_mgr::level_order_update() {
  return_if (m_level);
  accept_maker(); 
  if (cauto name = level_name(); !name.empty())
    hpw_log("выбран уровень: \"" + name + "\"\n", Log_stream::debug);
}

void Level_mgr::on_end_level() {
  // завершить все задачи
  hpw::task_mgr.kill_all();
  // освободить память от объектов
  if (hpw::entity_mgr)
    hpw::entity_mgr->clear();
  // для смены уровня в level_order_update
  m_level = {};
}

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
  // если уровней в очереди нет, то поставить флаг
  if (m_makers.empty()) {
    end_of_levels = true;
    return;
  }

  // взять следующий уровень и убрать его из списка на создание
  auto maker = m_makers.front();
  m_level = maker();
  m_makers.pop_front();
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
  return_if (m_level, m_level->level_name());
  hpw_log("empty m_level\n", Log_stream::debug);
  return {};
}

void Level_mgr::set_visible(bool mode) { m_visible = mode; }

void Level_mgr::set(cr<Level_mgr::Maker> maker) { 
  assert(maker);
  if (m_level)
    on_end_level();
  m_level = maker();
}
