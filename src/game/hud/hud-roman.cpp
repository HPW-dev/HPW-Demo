#include <cassert>
#include <cmath>
#include "hud-roman.hpp"
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
#include "util/str-util.hpp"
#include "util/math/mat.hpp"

struct Hud_roman::Impl {
  constx std::int64_t MAX_ROMAN_NUMBER = 3'999;
  Hud& _master;
  Vec _pos_hp    {5,   graphic::height-17};
  Vec _pos_en    {190, graphic::height-17};
  Vec _pos_score {380, graphic::height-17};
  //Rect _en_rect {};
  //Rect _hp_rect {};
  //Rect _pts_rect {};
  //Rect _player_rect {};

  inline Impl(Hud& master): _master{master} {}

  inline void update(const Delta_time dt) {
    resolve_collisions();
  }

  inline void draw(Image& dst) const {
    cauto player = hpw::entity_mgr->get_player();
    return_if (!player);
    draw_roman(dst, *player);

    if (graphic::draw_hitboxes)
      debug_draw();
  }
  
  // переводит число в римские цифры
  static Str to_roman(std::int64_t input) {
    assert(input <= MAX_ROMAN_NUMBER);

    constexpr char ROMAN_ZERO = 'N';
    return_if (input == 0, Str{ROMAN_ZERO});

    const Str SUB_SYMBOL {input < 0 ? "-" : ""};
    input = std::abs(input);

    static const Strs ones {"","I","II","III","IV","V","VI","VII","VIII","IX"};
    static const Strs tens {"","X","XX","XXX","XL","L","LX","LXX","LXXX","XC"};
    static const Strs hunds {"","C","CC","CCC","CD","D","DC","DCC","DCCC","CM"};
    static const Strs thous {"","M","MM","MMM","MMMM"};

    cauto t = thous.at( input / 1'000);
    cauto h = hunds.at((input / 100) % 10);
    cauto te = tens.at((input / 10)  % 10);
    cauto o =  ones.at( input        % 10);

    return SUB_SYMBOL + t + h + te + o;
  }

  // переводит число в расширенные римские цифры
  static Str to_big_roman(std::int64_t input) {
    constexpr char ROMAN_ZERO = 'N';
    return_if (input == 0, Str{ROMAN_ZERO});

    const Str SUB_SYMBOL {input < 0 ? "-" : ""};
    input = std::abs(input);
    return_if (input <= MAX_ROMAN_NUMBER, to_roman(input));

    // TODO
    return SUB_SYMBOL + "need impl";
  }
  
  inline void draw_roman(Image& dst, Player& player) const {
   cauto hp_ratio = safe_div(player.get_hp(), scast<real>(player.hp_max));
   cauto en_ratio = safe_div(player.energy, scast<real>(player.energy_max));
   cauto hp = hp_ratio * MAX_ROMAN_NUMBER;
   cauto energy = en_ratio * MAX_ROMAN_NUMBER;
   cauto score = hpw::get_score_normalized();
   Hud::draw_expanded_text(dst, utf8_to_32("SALUS - " + to_big_roman(hp)), _pos_hp);
   Hud::draw_expanded_text(dst, utf8_to_32("ENERGIA - " + to_big_roman(energy)), _pos_en);
   Hud::draw_expanded_text(dst, utf8_to_32("PECUNIA - " + to_big_roman(score)), _pos_score);
  }

  inline void debug_draw() const {
    assert(hpw::hitbox_layer);
    //draw_rect(*hpw::hitbox_layer, _hp_rect, Pal8::white);
    //draw_rect(*hpw::hitbox_layer, _en_rect, Pal8::white);
    //draw_rect(*hpw::hitbox_layer, _pts_rect, Pal8::white);
    //draw_rect(*hpw::hitbox_layer, _player_rect, Pal8::white);
  }

  inline void resolve_collisions() {
    /*
    return_if (hpw::difficulty == Difficulty::easy);
    cauto player = hpw::entity_mgr->get_player();
    return_if (!player);
    // хитбокс игрока
    auto player_pos = player->phys.get_pos();
    _player_rect = Rect(player_pos - Vec(15, 2), Vec(31, 17));
    // остальные хитбоксы
    _hp_rect = Rect(_pos_hp, Vec(126, 12));
    _en_rect = Rect(_pos_en, Vec(126, 12));
    _pts_rect = Rect(_pos_score, Vec(126, 12));
    // выталкивание игрока вверх
    if (intersect(_player_rect, _hp_rect)) {
      player_pos.y = _hp_rect.pos.y - _player_rect.size.y;
      player->set_pos(player_pos);
    }
    if (intersect(_player_rect, _en_rect)) {
      player_pos.y = _en_rect.pos.y - _player_rect.size.y;
      player->set_pos(player_pos);
    }
    if (intersect(_player_rect, _pts_rect)) {
      player_pos.y = _pts_rect.pos.y - _player_rect.size.y;
      player->set_pos(player_pos);
    }
   */
  }
}; // Impl

void Hud_roman::draw(Image& dst) const { impl->draw(dst); }
void Hud_roman::update(const Delta_time dt) { impl->update(dt); }
Hud_roman::Hud_roman(): impl{new_unique<Impl>(*this)} {}
Hud_roman::~Hud_roman() {}
