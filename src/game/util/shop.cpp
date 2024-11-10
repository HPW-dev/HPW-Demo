#include <cassert>
#include "shop.hpp"
#include "score-table.hpp"
#include "graphic/util/graphic-util.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/font/font-util.hpp"
#include "game/core/fonts.hpp"
#include "game/core/canvas.hpp"
#include "game/core/graphic.hpp"
#include "game/util/game-util.hpp"
#include "util/math/random.hpp"
#include "util/math/vec.hpp"
#include "util/math/timer.hpp"

struct Shopping_item {
  hpw::Score_out price {}; // сколько стоит предмет
  utf32 name {}; // название покупаемой способности
  Rect hitbox {}; // хитбокс окна
  bool selected {}; // текущий предметр выбран
};

struct Shop_task::Impl {
  constx hpw::Score_out FIRST_PRICE = 100; // первая цена, когда у игрока <= 0 очков
  constx std::size_t SHOPPING_ITEMS = 3; // сколько дают предметов на выбор
  constx real SELECT_DELAY = 5; // столько секунд надо подождать, чтобы выбрать способность

  Vector<Shopping_item> _shopping_items {};
  Timer _select_delay {};

  inline Impl() {
    init_prices();
    bind_hitboxes();
    bind_abilities();
    assert(_shopping_items.size() == 3); // система рассчитана на 3 способности в списке
  };

  inline void update(const Delta_time dt) {}

  inline void draw_post_bg(Image& dst) const {
    draw_items(dst);
  }

  inline void init_prices() {
    // эталонные очки для сравнения
    hpw::Score_out score = hpw::get_score_normalized();
    if (score <= 0)
      score = FIRST_PRICE;
    
    // накрутка цен
    _shopping_items.resize(SHOPPING_ITEMS);
    { // x = score / 2 * (-50% . . +25%)
      auto& item = _shopping_items.at(0); // дешман
      hpw::Score_out price = std::max<hpw::Score_out>(1, score / 2);
      price = rnd(price / 2, price + (price / 4));
      item.price = price;
    }
    { // y = (x + 1) . . score + 25%
      auto& item = _shopping_items.at(1); // можно купить, а можно не купить
      cauto PRICE_0 = _shopping_items.at(0).price;
      item.price = rnd(PRICE_0 + 1, score + (score / 4));
    }
    { // z = score * (-25% . . +75%)
      auto& item = _shopping_items.at(2); // трудно купить
      cauto ALPHA = std::max<hpw::Score_out>(1, score / 4) * 3;
      item.price = rnd(ALPHA, score + ALPHA);
    }
  }

  inline void bind_abilities() {
    // TODO добавить реальное связывание
    _shopping_items.at(0).name = U"тест 1";
    _shopping_items.at(1).name = U"тест два";
    _shopping_items.at(2).name = U"тест III";
  }

  inline void draw_items(Image& dst) const {
    for (crauto item: _shopping_items) {
      draw_item_wnd(dst, item.hitbox, item.selected);
      draw_item_text(dst, item);
    }
  }

  inline void draw_item_wnd(Image& dst, const Rect wnd, bool selected) const {
    assert(dst.X >= wnd.size.x + wnd.pos.x);
    assert(dst.Y >= wnd.size.y + wnd.pos.y);

    static Image croped_bg;
    croped_bg.init(wnd.size.x, wnd.size.y);

    // фон затенить и заблюрить
    fast_cut_2(croped_bg, dst, wnd.pos.x, wnd.pos.y, wnd.size.x, wnd.size.y);
    if (check_high_blur()) {
      static Image blured_bg;
      blured_bg.init(wnd.size.x, wnd.size.y);
      hpw_blur(blured_bg, croped_bg, 5);
      apply_brightness(blured_bg, -20);
      insert(dst, blured_bg, wnd.pos);
    } else { // обойтись без блюра
      apply_brightness(croped_bg, -20);
      insert(dst, croped_bg, wnd.pos);
    }

    const Rect RECT(wnd.pos - 1, wnd.size + 2);
    draw_rect<blend_diff>(dst, RECT, selected ? Pal8::white : Pal8::gray);
  }

  inline void bind_hitboxes() {
    const Vec ITEM_SZ(graphic::width - 100, 70);
    const Vec OFFSET((graphic::width - ITEM_SZ.x) / 2, (graphic::height - (ITEM_SZ.y * 3)) / 2);

    cfor (i, _shopping_items.size())
      _shopping_items[i].hitbox = Rect(OFFSET + Vec(0, ITEM_SZ.y * i - i), ITEM_SZ);
  }

  inline void draw_item_text(Image& dst, cr<Shopping_item> item) const {
    cauto txt = item.name; // TODO сменяется на price
    const Vec TEXT_OFFSET(18, (item.hitbox.size.y - graphic::font_shop->h()) / 2);
    text_bordered(dst, txt, graphic::font_shop.get(), item.hitbox, TEXT_OFFSET, &blend_max);
  }
}; // Impl

Shop_task::Shop_task(): impl{new_unique<Impl>()} {}
Shop_task::~Shop_task() {}
void Shop_task::update(const Delta_time dt) { impl->update(dt); }
void Shop_task::draw_post_bg(Image& dst) const { impl->draw_post_bg(dst); }
