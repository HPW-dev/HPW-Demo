#include <cmath>
#include "hud-asci.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/util/graphic-util.hpp"
#include "graphic/font/font.hpp"
#include "game/entity/player.hpp"
#include "game/entity/player-dark.hpp"
#include "game/entity/entity-manager.hpp"
#include "game/util/score-table.hpp"
#include "game/game-canvas.hpp"
#include "game/game-common.hpp"
#include "game/game-font.hpp"
#include "game/game-sync.hpp"
#include "game/game-debug.hpp"
#include "util/math/mat.hpp"

struct Hud_asci::Impl {
  constx uint line_len {20};
  mutable Rect hp_rect {};
  mutable Rect en_rect {};
  mutable Rect pts_rect {};

  inline Impl() = default;

  inline void update(double dt) const {
    auto player = hpw::entity_mgr->get_player();
    return_if (!player);
    cauto hp_sz = load_bar_sz(player->get_hp(), player->hp_max, line_len);
    cauto en_sz = load_bar_sz(player->energy, player->energy_max, line_len);
    cauto ch_sz = 7u;
    hp_rect = Rect(0, 367, 27 + hp_sz * ch_sz, graphic::height - 367);
    en_rect = Rect(186, 367, 16 + en_sz * ch_sz, graphic::height - 367);
    pts_rect = Rect(361, 367, 31, graphic::height - 367);
  }

  inline void draw(Image& dst) const {
    auto player = hpw::entity_mgr->get_player();
    return_if (!player);

    // напечатать полоску hp
    utf32 hp_txt;
    hp_txt += U"HP:";
    cfor (_, load_bar_sz(player->get_hp(), player->hp_max, line_len))
      hp_txt += U'#';

    // энергия мигает, если её мало
    if ( !(player->energy <= 4 && (graphic::frame_count & 7))) {
      // напечатать полоску энергии
      utf32 en_txt = U"EN:";
      cfor (_, load_bar_sz(player->energy, player->energy_max, line_len))
        en_txt += U'#';
      draw_expanded_text(dst, en_txt, {185, dst.Y - (graphic::font->h() + 2)});
    }

    // напечатать очки
    utf32 pts_txt;
    pts_txt += U"PTS:" + n2s<utf32>(hpw::get_score());

    draw_expanded_text(dst, hp_txt, {10, dst.Y - (graphic::font->h() + 2)});
    draw_expanded_text(dst, pts_txt, {360, dst.Y - (graphic::font->h() + 2)});

    if (graphic::draw_hitboxes)
      debug_draw();
  } // draw

  inline int load_bar_sz(double val, double max, int size_bar) const {
    return std::ceil(safe_div<double, double>(val, max) * size_bar);
  }

  /// рисует прозрачный текст с чёрными контурами
  inline void draw_expanded_text(Image& dst, CN<utf32> txt, const Vec pos) const {
    // рендер в буфферы под текст
    Image hp_overlay(graphic::font->text_width(txt) + 2,
      graphic::font->text_height(txt) + 2, Pal8::black);
    graphic::font->draw(hp_overlay, {1, 1}, txt);

    // расширение контуров текста
    Image hp_overlay_black(hp_overlay);
    apply_invert(hp_overlay_black);
    expand_color_8(hp_overlay_black, Pal8::black);

    // вставка тёмного контура текста
    insert_blink<&blend_min>(dst, hp_overlay_black, pos, graphic::frame_count);
    // вставка текста
    insert_blink<&blend_max>(dst, hp_overlay, pos, graphic::frame_count);
  } // draw_expanded_text

  inline void debug_draw() const {
    assert(hpw::hitbox_layer);
    draw_rect(*hpw::hitbox_layer, hp_rect, Pal8::white);
    draw_rect(*hpw::hitbox_layer, en_rect, Pal8::white);
    draw_rect(*hpw::hitbox_layer, pts_rect, Pal8::white);
  }

}; // Impl

void Hud_asci::draw(Image& dst) const { impl->draw(dst); }
void Hud_asci::update(double dt) const { impl->update(dt); }
Hud_asci::Hud_asci(): impl{new_unique<Impl>()} {}
Hud_asci::~Hud_asci() {}
