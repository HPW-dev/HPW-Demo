#include <cmath>
#include "hud-asci.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/util/graphic-util.hpp"
#include "game/entity/player/player.hpp"
#include "game/entity/player/player-dark.hpp"
#include "game/util/score-table.hpp"
#include "game/util/sync.hpp"
#include "game/util/camera.hpp"
#include "game/entity/util/entity-util.hpp"
#include "game/core/entities.hpp"
#include "game/core/canvas.hpp"
#include "game/core/fonts.hpp"
#include "game/core/debug.hpp"
#include "game/core/difficulty.hpp"
#include "util/math/mat.hpp"

struct Hud_asci::Impl {
  Hud& _master;
  constx uint line_len {20};
  Rect hp_rect {};
  Rect en_rect {};
  Rect pts_rect {};
  Rect hp_rect_old {};
  Rect en_rect_old {};
  Rect pts_rect_old {};
  Rect player_rect {};

  inline Impl(Hud& master)
  : _master{master}
  , hp_rect(0, 367, 27, graphic::height - 367)
  , en_rect(186, 367, 16, graphic::height - 367)
  , pts_rect(361, 367, 31, graphic::height - 367)
  {
    hp_rect_old = hp_rect;
    en_rect_old = en_rect;
    pts_rect_old = pts_rect;
  }

  inline void update(const Delta_time dt) {
    return_if (hpw::difficulty == Difficulty::easy);
    
    auto player = hpw::entity_mgr->get_player();
    return_if (!player);

    update_hud_rects(player);
    cauto _player_rect = get_player_rect();
    return_if (!_player_rect);
    player_rect = _player_rect.value();

    push_player(player);
    check_crush(player);
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
      Hud::draw_expanded_text(dst, en_txt, {185, dst.Y - (graphic::font->h() + 2)});
    }

    // напечатать очки
    utf32 pts_txt;
    pts_txt += U"PTS:" + n2s<utf32>(hpw::get_score_normalized());

    Hud::draw_expanded_text(dst, hp_txt,  {10,  dst.Y - (graphic::font->h() + 2)});
    Hud::draw_expanded_text(dst, pts_txt, {360, dst.Y - (graphic::font->h() + 2)});

    if (graphic::draw_hitboxes)
      debug_draw();
  } // draw

  inline int load_bar_sz(real val, real max, int size_bar) const {
    return std::ceil(safe_div<real, real>(val, max) * size_bar);
  }

  inline void debug_draw() const {
    assert(hpw::hitbox_layer);
    draw_rect(*hpw::hitbox_layer, hp_rect, Pal8::white);
    draw_rect(*hpw::hitbox_layer, en_rect, Pal8::white);
    draw_rect(*hpw::hitbox_layer, pts_rect, Pal8::white);
    draw_rect(*hpw::hitbox_layer, player_rect, Pal8::white);
  }

  // выпихивает игрока из надписей на интерфейсе
  inline void push_player(Player* player) const {
    push_player_helper(player, hp_rect, hp_rect_old);
    push_player_helper(player, pts_rect, pts_rect_old);
    push_player_helper(player, en_rect, en_rect_old);
  }

  inline void push_player_helper(Player* player, const Rect rect,
  const Rect rect_old) const {
    return_if( !intersect(player_rect, rect));
    cauto pos = player->phys.get_pos();
    // левый нижний угол игрока
    const Vec player_ld_side(
      player_rect.pos.x,
      player_rect.pos.y + player_rect.size.y
    );
    // правый верхний угол полоски
    const Vec rect_old_ur_side(
      rect_old.pos.x + rect_old.size.x,
      rect_old.pos.y
    );

    if ((player_ld_side.y >= rect_old_ur_side.y) &&
        (player_ld_side.x >= rect_old_ur_side.x))
    {
      // толкать вправо
      player->phys.set_pos(Vec(rect.pos.x + rect.size.x + 15, pos.y));
    } else {
      // толкать вверх
      player->phys.set_pos(Vec(pos.x, graphic::height - 32));
    }
  } // push_player_helper

  // убивает игрока, если его придавят полоски
  inline void check_crush(Player* player) const {
    bool killme = intersect(player_rect, en_rect) && intersect(player_rect, pts_rect);
    // не взрывать игрока на такой высоте
    if (player_rect.pos.y + player_rect.size.y <= en_rect.pos.y + 3)
      killme = false;
    // при взрыве не надо убивать игрока
    if (graphic::camera && graphic::camera->shaked())
      killme = false;
    if (killme)
      player->sub_hp(player->get_hp());
  }

  inline void update_hud_rects(Player* player) {
    cauto hp_sz = load_bar_sz(player->get_hp(), player->hp_max, line_len);
    cauto en_sz = load_bar_sz(player->energy, player->energy_max, line_len);
    cauto ch_sz = 7u;
    hp_rect_old = hp_rect;
    en_rect_old = en_rect;
    pts_rect_old = pts_rect;
    hp_rect = Rect(0, 367, 27 + hp_sz * ch_sz, graphic::height - 367);
    en_rect = Rect(186, 367, 16 + en_sz * ch_sz, graphic::height - 367);
    pts_rect = Rect(361, 367, 24 + digits_number_i32(hpw::get_score_normalized()) * ch_sz,
      graphic::height - 367);
  } // update_hud_rects

}; // Impl

void Hud_asci::draw(Image& dst) const { impl->draw(dst); }
void Hud_asci::update(const Delta_time dt) { impl->update(dt); }
Hud_asci::Hud_asci(): impl{new_unique<Impl>(*this)} {}
Hud_asci::~Hud_asci() {}
