#include <functional>
#include "level-tutorial.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/graphic-util.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/font/font.hpp"
#include "game/core/common.hpp"
#include "game/core/entities.hpp"
#include "game/util/game-util.hpp"
#include "game/core/levels.hpp"
#include "game/entity/entity-manager.hpp"
#include "game/entity/collider/collider-qtree.hpp"
#include "game/level/level-manager.hpp"
#include "game/core/fonts.hpp"
#include "game/core/graphic.hpp"
#include "game/core/canvas.hpp"
#include "game/core/core.hpp"
#include "game/util/keybits.hpp"
#include "util/math/random.hpp"
#include "util/math/vec.hpp"
#include "util/math/timer.hpp"
#include "host/command.hpp"

class Button final {
public:
  /// функция проверки что кнопка нажата
  using test_key_pf = std::function<bool ()>;
  /// функция получающая название кнопки
  using get_name_key_pf = std::function<utf32 ()>;

  Button() = default;
  explicit Button(const Vec pos, get_name_key_pf&& get_name_key_f,
    test_key_pf&& test_key_f, CN<utf32> description);
  void update();
  void draw(Image& dst) const;

private:
  test_key_pf m_test_key_f {};
  get_name_key_pf m_get_name_key_f {};
  Vec m_pos {};
  utf32 m_description {};
  bool m_activated {false}; /// запускает подсветку кнопки
};

struct Level_tutorial::Impl {

  Vector<Button> m_buttons {};
  hpw::keycode key_1 {hpw::keycode::up}; /// 1 кнопка для выхода с уровня
  hpw::keycode key_2 {hpw::keycode::down}; /// 2 кнопка для выхода с уровня

  Light m_light {};
  Vec m_light_pos {};

  inline explicit Impl() {
    hpw::shmup_mode = true;
    init_buttons();
    init_collider();
    make_player();
  }

  inline void update(const Vec vel, double dt) {
    for (nauto button: m_buttons)
      button.update();
    
    // выйти по нажатию двух кнопок
    if (is_pressed(key_1) && is_pressed(key_2))
      hpw::level_mgr->finalize_level();
  }

  inline void draw(Image& dst) const {
    dst.fill(Pal8::black);
    draw_how2start_banner(dst);
  }

  inline void draw_upper_layer(Image& dst) const {
    for (cnauto button: m_buttons)
      button.draw(dst);
  }

  /// рисует мигающую табличку с кнопками для старта
  inline void draw_how2start_banner(Image& dst) const {
    // надпись
    utf32 txt = U"чтобы начать игру, зажмите ";
    cauto keys_info = hpw::keys_info;
    if (is_pressed(key_1))
      txt += U">" + keys_info.find(key_1)->name;
    else
      txt += keys_info.find(key_1)->name;
    txt += U" и ";
    if (is_pressed(key_2))
      txt += U">" + keys_info.find(key_2)->name;
    else
      txt += keys_info.find(key_2)->name;
    auto txt_sz = graphic::font->text_size(txt);

    // отрисовка надписи в прямоугольник и инвертирование цвета
    Image banner(txt_sz.x + 5, txt_sz.y + 5, Pal8::red_black);
    graphic::font->draw(banner, Vec(5, 5), txt);
    
    // мигание
    if ((hpw::game_updates_safe % 250) > 125u)
      apply_invert(banner);

    Vec pos((dst.X - banner.X) / 2, 340);
    insert(dst, banner, pos);
  } // draw_how2start_banner

  inline void make_player()
    { hpw::entity_mgr->make({}, "player.boo.dark", get_screen_center()); }

  inline void init_collider() {
    hpw::entity_mgr->set_collider(new_shared<Collider_qtree>(6, 1,
      graphic::width, graphic::height));
  }

  inline void init_buttons() {
    #define add_button(x, y, key_name) \
      Button ( \
        Vec(x, y), \
        []->utf32 { return hpw::keys_info.find(hpw::keycode::key_name)->name; }, \
        []->bool { return is_pressed(hpw::keycode::key_name); }, \
        get_locale_str("scene.input."#key_name) \
      ),

    Vec buttons_offset(20, 20);

    m_buttons = {
      add_button(buttons_offset.x, buttons_offset.y + 0 * 20, escape)
      add_button(buttons_offset.x, buttons_offset.y + 1 * 20, enable)

      add_button(buttons_offset.x + 250, buttons_offset.y + 0 * 20, screenshot)
      add_button(buttons_offset.x + 250, buttons_offset.y + 1 * 20, fulscrn)

      add_button(buttons_offset.x, buttons_offset.y + 3 * 20, up)
      add_button(buttons_offset.x, buttons_offset.y + 4 * 20, down)
      add_button(buttons_offset.x, buttons_offset.y + 5 * 20, left)
      add_button(buttons_offset.x, buttons_offset.y + 6 * 20, right)
      
      add_button(buttons_offset.x + 250, buttons_offset.y + 3 * 20, shoot)
      add_button(buttons_offset.x + 250, buttons_offset.y + 4 * 20, focus)
    }; // init buttons vector

    #undef add_button
  } // init_buttons

}; // Impl

Button::Button( const Vec pos, Button::get_name_key_pf&& get_name_key_f,
Button::test_key_pf&& test_key_f, CN<utf32> description )
: m_test_key_f {test_key_f}
, m_get_name_key_f {get_name_key_f}
, m_pos {pos}
, m_description {description}
{
  assert(m_get_name_key_f);
  assert(m_test_key_f);
}

void Button::update() { m_activated = m_test_key_f(); }

void Button::draw(Image& dst) const {
  // нарисовать надпись кнопки
  auto name = m_get_name_key_f();
  graphic::font->draw(dst, m_pos, name, &blend_diff);

  // закрасить кнопку
  Rect button_rect(
    m_pos - Vec(4, 4),
    graphic::font->text_size(name) + Vec(8, 2)
  );
  if (m_activated)
    draw_rect_filled<&blend_diff>(dst, button_rect, Pal8::white);
  else
    draw_rect<&blend_diff>(dst, button_rect, Pal8::white);

  // напечатать описание кнопки
  graphic::font->draw(dst, m_pos + Vec(button_rect.size.x, 0), m_description, &blend_diff);
} // draw

Level_tutorial::Level_tutorial(): impl {new_unique<Impl>()} {}
Level_tutorial::~Level_tutorial() {}
void Level_tutorial::update(const Vec vel, double dt) {
  Level::update(vel, dt);
  impl->update(vel, dt);
}
void Level_tutorial::draw(Image& dst) const { impl->draw(dst); }
void Level_tutorial::draw_upper_layer(Image& dst) const { impl->draw_upper_layer(dst); }
