#include <ranges>
#include <cassert>
#include "scene-nickname.hpp"
#include "game/core/user.hpp"
#include "game/core/fonts.hpp"
#include "game/core/scenes.hpp"
#include "game/core/common.hpp"
#include "game/util/sync.hpp"
#include "game/util/game-util.hpp"
#include "game/util/keybits.hpp"
#include "graphic/font/font-util.hpp"

struct Scene_nickname::Impl {

  inline Impl() {
    hpw::text_input = hpw::player_name;
    hpw::text_input_pos = hpw::text_input.size();
    hpw::text_input_mode = true;
  }

  inline ~Impl() {
    hpw::text_input_mode = false;
  }

  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape))
      exit();

    // стирание текста
    if (is_pressed_once(hpw::keycode::text_delete)) {
      if (!hpw::text_input.empty()) {
        hpw::text_input_pos = std::clamp<int>(hpw::text_input_pos - 1,
          0, hpw::text_input.size());
        hpw::text_input.erase(hpw::text_input_pos, 1);
      }
    }

    // перемещение текстового курсора ввода влево
    if (is_pressed_once(hpw::keycode::left)) {
      hpw::text_input_pos = std::clamp<int>(hpw::text_input_pos - 1,
        0, hpw::text_input.size());
    }

    // перемещение текстового курсора ввода вправо
    if (is_pressed_once(hpw::keycode::right)) {
      hpw::text_input_pos = std::clamp<int>(hpw::text_input_pos + 1,
        0, hpw::text_input.size());
    }

    // подтвердить и выйти
    if (is_pressed_once(hpw::keycode::enable)) {
      hpw::player_name = hpw::text_input;
      exit();
    }
  }

  inline void draw(Image& dst) const {
    dst.fill({});
    utf32 txt = get_locale_str("scene.nickname.enter_name") + U": ";
    txt += hpw::text_input;
    if ((graphic::frame_count % 60) > 30)
      txt += U"_";
    assert(graphic::font);
    graphic::font->draw(dst, Vec(25, dst.Y/3 - 6), txt);

    auto info_for_exit = get_locale_str("scene.nickname.press_key") + U" ";
    info_for_exit += hpw::keys_info.find(hpw::keycode::enable)->name + U" ";
    info_for_exit += get_locale_str("scene.nickname.for_exit");
    graphic::font->draw(dst, Vec(25, dst.Y/3 + 12), info_for_exit);
  }

  inline static void exit() {
    hpw::text_input_mode = false;
    hpw::scene_mgr->back();
  }
}; // impl

Scene_nickname::Scene_nickname(): impl {new_unique<Impl>()} {}
Scene_nickname::~Scene_nickname() {}
void Scene_nickname::update(const Delta_time dt) { impl->update(dt); }
void Scene_nickname::draw(Image& dst) const { impl->draw(dst); }

