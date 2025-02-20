#include <cassert>
#include <cmath>
#include "advanced-text-menu.hpp"
#include "graphic/image/image.hpp"
#include "graphic/font/font-util.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/util/graphic-util.hpp"
#include "item/item.hpp"
#include "game/core/fonts.hpp"
#include "game/util/vec-helper.hpp"

struct Advanced_text_menu::Impl {
  Menu* m_base {};
  utf32 m_title {}; // название всего меню
  Vec m_title_pos {};
  Rect m_rect {}; // основная рамка меню
  Rect m_items_rect {}; // область для отрисовки пунктов меню
  Rect m_desc_rect {}; // область для отрисовки описания пунктов
  constx real ITEMS_DESC_RATIO {1.0 / 3.5}; // соотношение области пунктов и описания
  constx real SCROLL_SPEED {8.0}; // скорость прокрутки текста в меню
  real m_items_h_offset {}; // для плавной прокрутки по тексту
  Advanced_text_menu_config _config {};

  inline explicit Impl(Menu* base, cr<utf32> title, const Rect rect, cr<Advanced_text_menu_config> config)
  : m_base {base}
  , m_title {title}
  , m_rect {rect}
  , _config {config}
  {
    assert(m_base);
    assert( !m_title.empty());
    assert( !base->get_items().empty());

    if (m_rect.size.is_zero())
      m_rect = get_screen_rect();

    m_title_pos = m_rect.pos + Vec(7, 7);
    assert(m_title_pos.x < m_rect.size.x);
    assert(m_title_pos.y < m_rect.size.y);

    m_items_rect = Rect(
      m_rect.pos + Vec(0, 25),
      m_rect.size - Vec(0, 25 + m_rect.size.y * (_config.without_desc ? 0 : ITEMS_DESC_RATIO))
    );
    assert(m_items_rect.size.y >= 11);
    
    if (!_config.without_desc) {
      m_desc_rect = Rect (
        Vec(m_rect.pos.x, m_items_rect.pos.y + m_items_rect.size.y - 1),
        Vec(m_rect.size.x, m_rect.size.y - m_items_rect.size.y - 24)
      );
      assert(m_desc_rect.size.y >= 11);
    }
  } // Impl c-tor

  inline void draw(Image& dst) const {
    draw_bg(dst);
    draw_title(dst);
    draw_items(dst);
    draw_decription(dst);
  }

  inline void update(const Delta_time dt) {
    m_items_h_offset = update_items_h_offset(dt);
  }

  inline void draw_bg(Image& dst) const {
    draw_rect_filled(dst, m_rect, _config.color_bg, _config.bf_bg, {});
    draw_rect(dst, m_rect, _config.color_border, _config.bf_border, {}); // рамка меню
    draw_rect(dst, m_items_rect, _config.color_border, _config.bf_border, {}); // рамка пунктов меню

    if (!_config.without_desc)
      draw_rect(dst, m_desc_rect, _config.color_border, _config.bf_border, {}); // рамка описания
  }

  inline void draw_title(Image& dst) const {
    graphic::font->draw(dst, m_title_pos, m_title);
  }

  inline void draw_items(Image& dst) const {
    // нарисовать весь текст в прямоугольник
    Image items_rect(m_items_rect.size.x, m_items_rect.size.y, Pal8::black);

    crauto items = m_base->get_items();
    cfor (i, items.size()) {
      crauto item = items[i];
      assert(item);

      utf32 str = item->to_text();
      // если строка выбрана, то поставить указатель
      if (i == m_base->get_cur_item_id())
        str += U" <";

      cauto text_offset_h = i * (graphic::font->text_height(str) + 2);
      graphic::font->draw(items_rect,
        Vec(14, 11 + text_offset_h - m_items_h_offset), str);
    }

    cauto crop_rect = Rect(Vec(1, 1), m_items_rect.size - Vec(2, 2));
    // обрезать символы возле рамки
    draw_rect(items_rect, crop_rect, Pal8::black);
    insert<&blend_max>(dst, items_rect, m_items_rect.pos);
  }

  inline void draw_decription(Image& dst) const {
    return_if(_config.without_desc);
    auto item = m_base->get_cur_item();
    assert(item);
    text_bordered(dst, item->get_description(), graphic::font.get(), m_desc_rect, {9, 11});
  }
  
  // скроллинг по тексту пунктов
  inline real update_items_h_offset(const Delta_time dt) {
    cauto item_h = (graphic::font->h() + 2);
    const real items_h = item_h * m_base->get_items().size();
    const real menu_h = m_items_rect.size.y;
    return_if (items_h <= menu_h, 0);

    const real pos_h = item_h * m_base->get_cur_item_id();
    // двигать камеру, когда курсор ниже середины меню
    const real ret = std::lerp(m_items_h_offset, pos_h - menu_h / 2.0,
      dt * SCROLL_SPEED);
    cauto crop_h = items_h - menu_h + item_h;
    assert(crop_h > item_h);
    return std::clamp<real>(ret, 0, crop_h);
  }
}; // impl

Advanced_text_menu::Advanced_text_menu(cr<utf32> title, cr<Menu_items> items,
const Rect rect, cr<Advanced_text_menu_config> config)
: Menu {items}
, impl {new_unique<Impl>(this, title, rect, config)}
{}

void Advanced_text_menu::draw(Image& dst) const { impl->draw(dst); }

void Advanced_text_menu::update(const Delta_time dt) {
  Menu::update(dt);
  impl->update(dt);
}

Advanced_text_menu::~Advanced_text_menu() {}
