#include <cmath>
#include <stdfloat>
#include "hud-hex.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/util/graphic-util.hpp"
#include "graphic/font/font-util.hpp"
#include "game/entity/player/player.hpp"
#include "game/util/score-table.hpp"
#include "game/util/sync.hpp"
#include "game/util/camera.hpp"
#include "game/util/score-table.hpp"
#include "game/core/entities.hpp"
#include "game/core/canvas.hpp"
#include "game/core/fonts.hpp"
#include "game/core/debug.hpp"
#include "game/core/difficulty.hpp"
#include "util/math/mat.hpp"
#include "util/str-util.hpp"

struct Hud_hex::Impl {
  Hud& _master;
  Vec _pos_hp    {5,   graphic::height-17};
  Vec _pos_en    {190, graphic::height-17};
  Vec _pos_score {380, graphic::height-17};

  inline Impl(Hud& master)
  : _master{master}
  {
  }

  inline void update(const Delta_time dt) {
    return_if (hpw::difficulty == Difficulty::easy);
    cauto player = hpw::entity_mgr->get_player();
    return_if (!player);


  }

  inline void draw(Image& dst) const {
    cauto player = hpw::entity_mgr->get_player();
    return_if (!player);
    draw_hex(dst, *player);

    if (graphic::draw_hitboxes)
      debug_draw();
  }
  
  inline void draw_hex(Image& dst, Player& player) const {
    cauto hp_ratio = safe_div(player.get_hp(), scast<std::float128_t>(player.hp_max));
    cauto en_ratio = safe_div(player.energy, scast<std::float128_t>(player.energy_max));
    const std::uint64_t hp_val = hp_ratio * 0xFFFF'FFFF'FFFF'FFFFu;
    const std::uint64_t en_val = en_ratio * 0xFFFF'FFFF'FFFF'FFFFu;
    cauto score = hpw::get_score();
    Hud::draw_expanded_text(dst, utf8_to_32(n2hex(hp_val)), _pos_hp);
    Hud::draw_expanded_text(dst, utf8_to_32(n2hex(en_val)), _pos_en);
    Hud::draw_expanded_text(dst, utf8_to_32(n2hex(score)), _pos_score);
  }

  inline void debug_draw() const {
    assert(hpw::hitbox_layer);
    //draw_rect(*hpw::hitbox_layer, hp_rect, Pal8::white);
    //draw_rect(*hpw::hitbox_layer, en_rect, Pal8::white);
    //draw_rect(*hpw::hitbox_layer, pts_rect, Pal8::white);
    //draw_rect(*hpw::hitbox_layer, player_rect, Pal8::white);
  }
}; // Impl

void Hud_hex::draw(Image& dst) const { impl->draw(dst); }
void Hud_hex::update(const Delta_time dt) { impl->update(dt); }
Hud_hex::Hud_hex(): impl{new_unique<Impl>(*this)} {}
Hud_hex::~Hud_hex() {}
