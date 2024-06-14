#include <cassert>
#include <filesystem>
#include <utility>
#include <functional>
#include "scene-cmd.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/util/graphic-util.hpp"
#include "game/core/user.hpp"
#include "game/core/common.hpp"
#include "game/core/canvas.hpp"
#include "game/core/fonts.hpp"
#include "game/core/scenes.hpp"
#include "game/core/messages.hpp"
#include "game/util/sync.hpp"
#include "game/util/keybits.hpp"
#include "game/util/cmd.hpp"
#include "game/util/game-util.hpp"

struct Scene_cmd::Impl {
  Image bg {};

  inline ~Impl() { hpw::text_input_mode = false; }

  inline Impl() {
    hpw::text_input_mode = true;
    hpw::text_input.clear();
    // фон - предыдущий кадр
    bg = *graphic::canvas;
  }

  inline void update(const Delta_time dt) {
    // выход
    if (is_pressed_once(hpw::keycode::escape))
      hpw::scene_mgr->back();
    if (is_pressed_once(hpw::keycode::console))
      hpw::scene_mgr->back();
    // стирание текста
    if (is_pressed_once(hpw::keycode::text_delete))
      if (!hpw::text_input.empty())
        hpw::text_input.resize(hpw::text_input.size() - 1);
    // выполнить команду
    if (is_pressed_once(hpw::keycode::enable)) {
      hpw::message_mgr->clear(); // убрать предыдущее сообщение
      hpw::cmd->exec( sconv<Str>(hpw::text_input) );
      hpw::text_input.clear();
    }
    // загрузить предыдущую команду
    if (is_pressed_once(hpw::keycode::up))
      hpw::text_input = sconv<utf32>(hpw::cmd->last_command());
  } // update

  inline void draw(Image& dst) const {
    insert_fast(dst, bg);
    sub_brightness(dst, 30);
    hpw::message_mgr->draw(dst);
    sub_brightness(dst, 110);
    print_input(dst);
  }

  inline void print_input(Image& dst) const {
    const Vec pos(15, 15);
    utf32 text = U"Команда: " + hpw::text_input;
    // мигающий курсор
    if ((graphic::frame_count % 30) >= 15)
      text += U'<';
    graphic::font->draw(dst, pos, text);
  }
}; // impl

Scene_cmd::Scene_cmd(): impl {new_unique<Impl>()} {}
Scene_cmd::~Scene_cmd() {}
void Scene_cmd::update(const Delta_time dt) { impl->update(dt); }
void Scene_cmd::draw(Image& dst) const { impl->draw(dst); }
