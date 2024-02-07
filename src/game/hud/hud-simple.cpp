#include "hud-simple.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/font/font.hpp"
#include "game/entity/player-dark.hpp"
#include "game/entity/player.hpp"
#include "game/entity/entity-manager.hpp"
#include "game/util/score-table.hpp"
#include "game/game-common.hpp"
#include "game/game-font.hpp"
#include "game/game-sync.hpp"
#include "game/util/game-util.hpp"
#include "util/math/mat.hpp"

void draw_loadbar(Image& dst, const Vec pos, uint val, uint max) {
  assert(val <= max);
  Rect bg(pos.x, pos.y, 130, 7);
  auto ratio = safe_div<double, double>(val, max);
  Image loadbar(bg.size.x * ratio - 1, bg.size.y - 1, Pal8::white);
  insert_blink(dst, loadbar, pos + Vec(1, 1), graphic::frame_count);
}

void draw_hud_1(Image& dst) {
  constexpr auto symbol_offset_y = 3;

  auto draw_hp = [](Image& dst, const Vec offset) {
    utf32 symbol = U"♡";
    Vec pos(5, symbol_offset_y);
    graphic::font->draw(dst, pos + offset, symbol);
    auto player = hpw::entity_mgr->get_player();
    draw_loadbar(dst, offset + pos + Vec(10, 2), player->get_hp(), player->hp_max);
  };
  auto draw_en = [](Image& dst, const Vec offset) {
    utf32 symbol = U"⚡";
    Vec pos(200, symbol_offset_y);
    graphic::font->draw(dst, pos + offset, symbol);
    auto player = hpw::entity_mgr->get_player();
    draw_loadbar(dst, offset + pos + Vec(9, 2), player->energy, player->energy_max);
  };
  auto draw_score = [](Image& dst, const Vec offset) {
    Vec pos(360, symbol_offset_y);
    auto score = hpw::get_score();
    utf32 txt = get_locale_str("common.money_symbol") + U' ' + n2s<utf32>(score);
    graphic::font->draw(dst, pos + offset, txt);
  };
  auto draw_bg = [](Image& dst, CN<Rect> bound) {
    Image bg_rect(bound.size.x, bound.size.y, Pal8::black);
    insert_blink(dst, bg_rect, bound.pos, graphic::frame_count);
  };

  Rect bound(0, dst.Y-19, dst.X, 19);
  static Image overlay;
  static Image overlay_dark;
  overlay.init(bound.size.x, bound.size.y);
  overlay_dark.init(bound.size.x, bound.size.y);

  draw_bg(dst, bound);
  draw_hp(dst, bound.pos);
  draw_en(dst, bound.pos);
  draw_score(dst, bound.pos);
} // draw_hud_1
