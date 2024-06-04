#include <cassert>
#include "scene-loading.hpp"
#include "game/core/fonts.hpp"
#include "game/core/scenes.hpp"
#include "game/core/sprites.hpp"
#include "game/scene/scene-manager.hpp"
#include "game/util/game-util.hpp"
#include "graphic/image/image.hpp"
#include "graphic/sprite/sprite.hpp"
#include "graphic/util/graphic-util.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/effect/dither.hpp"
#include "util/math/random.hpp"

Scene_loading::Scene_loading(std::function<void ()>&& _scene_maker)
: scene_maker {_scene_maker} {
  // найти в ресурсах загруженные картинки с фонами
  auto bg_names = hpw::store_sprite->list(true);
  std::erase_if(bg_names, [](CN<Str> src) {
    return src.find("resource/image/loading logo/") == Str::npos; });

  cauto bg_name = bg_names.at(rndu_fast(bg_names.size()));
  bg = hpw::store_sprite->find(bg_name).get();
  assert(bg);
}

void Scene_loading::update(const Delta_time dt) {
  // если сцена отработала, можно вернутся обратно
  if (used) {
    if (time_out-- <= 0) // защита на всякий случай
      hpw::scene_mgr->back();
  }

  if (drawed && !used) {
    scene_maker();
    used = true;
  }
} // update

void Scene_loading::draw(Image& dst) const {
  //return_if(used);
  cauto bg_image = bg->get_image();
  assert(bg_image);
  assert(dst.size == bg_image->size);

  drawed = true;

  // нарисовать фон
  insert_fast(dst, *bg_image);
  fast_dither_bayer16x16_4bit(dst);

  // нарисовать надпись с затенением
  cauto loading_txt = get_locale_str("common.loading");
  Image txt_overlay(dst.X, dst.Y, Pal8::black); // верхний белый слой текста
  Vec font_pos (
    (txt_overlay.X - graphic::font->text_width(loading_txt)) / 2.0,
    (txt_overlay.Y / 5.0) * 4
  );
  graphic::font->draw(txt_overlay, font_pos, loading_txt, &blend_max);

  Image txt_shadow(txt_overlay); // нижний слой с тенью текста
  apply_invert(txt_shadow);
  expand_color_8(txt_shadow, Pal8::black); // расширить контур тени
  expand_color_8(txt_shadow, Pal8::black);
  // нарисовать сначала тень, а поверх сам шрифт
  insert_fast<&blend_min>(dst, txt_shadow);
  insert_fast<&blend_max>(dst, txt_overlay);
} // draw
