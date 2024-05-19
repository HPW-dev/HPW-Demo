#include <cassert>
#include "game-shop-debug.hpp"
#include "graphic/image/image.hpp"
#include "game/menu/table-menu.hpp"
#include "game/menu/item/table-row-item.hpp"
#include "game/core/canvas.hpp"
#include "game/core/entities.hpp"
#include "game/entity/player.hpp"
#include "game/entity/entity-manager.hpp"

struct Shop_debug::Impl {
  Unique<Table_menu> item_table {};

  inline Impl() {
    item_table = new_unique<Table_menu>(
      U"Отладочное меню способностей",
      Table_menu::Rows {
        Table_menu::Row {.name = U"СПОСОБНОСТЬ", .sz = scast<uint>((graphic::width / 3.0) * 2)},
        Table_menu::Row {.name = U"ЦЕНА", .sz = 0},
      },
      24, gen_menu_rows()
    );
  }

  inline ~Impl() {}

  struct Ability_elem {
    Ability abil {}; // ability
    utf32 name {};
    utf32 desc {}; // description
  };

  inline Menu_items gen_menu_rows() {
    // TODO locale
    const Vector<Ability_elem> ability_table {
      Ability_elem {.abil = Ability::power_shoot, .name = U"Мощный выстрел", .desc = U"TODO DESC"},
      Ability_elem {.abil = Ability::invisible, .name = U"Невидимость", .desc = U"TODO DESC"},
      Ability_elem {.abil = Ability::graze_en_regen, .name = U"Реген маны за грейз", .desc = U"TODO DESC"},
      Ability_elem {.abil = Ability::stillness_en_regen, .name = U"Реген маны при неподвижности", .desc = U"TODO DESC"},
      Ability_elem {.abil = Ability::shield, .name = U"Силовое поле", .desc = U"TODO DESC"},
      Ability_elem {.abil = Ability::speedup_move, .name = U"Быстрое движение", .desc = U"TODO DESC"},
      Ability_elem {.abil = Ability::direct_shoot, .name = U"Все атаки прямые", .desc = U"TODO DESC"},
      Ability_elem {.abil = Ability::speedup_shoot, .name = U"Ускорение атак", .desc = U"TODO DESC"},
      Ability_elem {.abil = Ability::home, .name = U"Похищение противника", .desc = U"TODO DESC"},
      Ability_elem {.abil = Ability::fullscreen_shoot, .name = U"Стрельба на весь экран", .desc = U"TODO DESC"},
    };

    // TODO отображение описания способности
    Menu_items ret;
    for (cnauto it: ability_table) {
      ret.push_back(
        new_shared<Menu_item_table_row> (
          [=]{
            auto player = hpw::entity_mgr->get_player();
            assert(player);
            player->capabilities.push_back(it.abil);
          },
          Menu_item_table_row::Content_getters {
            [=]->utf32 { return it.name; },
            [=]->utf32 { return U"TODO price"; },
          }
        )
      );
    }
    return ret;
  }

  inline void draw(Image& dst) const {
    item_table->draw(dst);
  }

  inline bool update(const double dt) {
    item_table->update(dt);
    return !item_table->item_selected();
  }
}; // Impl

Shop_debug::Shop_debug(): impl {new_unique<Impl>()} {}
Shop_debug::~Shop_debug() {}
void Shop_debug::draw(Image& dst) const { impl->draw(dst); }
bool Shop_debug::update(const double dt) { return impl->update(dt); }
