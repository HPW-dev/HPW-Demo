#include <imgui.h>
#include "scene/sprite-select.hpp"
#include "util/hpw-util.hpp"
#include "util/str-util.hpp"
#include "game/scene/scene-manager.hpp"
#include "game/util/keybits.hpp"
#include "game/util/store.hpp"
#include "game/util/resource.hpp"
#include "game/core/scenes.hpp"
#include "game/core/fonts.hpp"
#include "game/core/sprites.hpp"
#include "host/command.hpp"
#include "graphic/image/image.hpp"
#include "graphic/sprite/sprite.hpp"
#include "graphic/font/font.hpp"
#include "graphic/util/graphic-util.hpp"
#include "graphic/util/util-templ.hpp"

Sprite_select::Sprite_select(decltype(callback) new_callback)
: callback(new_callback) {
  _list = hpw::store_sprite->list(true);
}

void Sprite_select::update(double dt) {
  if (is_pressed_once(hpw::keycode::escape))
    hpw::scene_mgr->back();
  if (is_pressed_once(hpw::keycode::enable))
    enable();

  if (is_pressed_once(hpw::keycode::up)) {
    auto list = get_list();
    auto idx = get_cur_list_idx(sel_spr, list);
    sel_spr = list.at(std::max<int>(0, idx-1));
  }

  if (is_pressed_once(hpw::keycode::down)) {
    auto list = get_list();
    auto idx = get_cur_list_idx(sel_spr, list);
    sel_spr = list.at(std::min<int>(idx+1, list.size()-1));
  }
} // update

void Sprite_select::draw(Image& dst) const {
  dst.fill(Pal8::from_real(0.25, true));

  return_if(sel_spr.empty());

  if (auto sprite = hpw::store_sprite->find(sel_spr); sprite) {
    auto center = center_point(dst, *sprite);
    insert(dst, *sprite, center);
    auto text = sconv<utf32>(sel_spr);
    graphic::font->draw(
      dst,
      Vec (
        center_point(dst).x - graphic::font->text_width(text) / 2,
        center.y + sprite->get_image()->Y + 5
      ),
      text,
      &blend_max
    );
  }
} // draw

void Sprite_select::imgui_exec() {
  using namespace ImGui;
  Begin("Sprite select", {}, ImGuiWindowFlags_AlwaysAutoResize);
  Scope _({}, &ImGui::End);
  PushItemWidth(400);

  InputText("filter##input", filter.data(), filter.size());
  auto src_list = get_list();

  // фильтр имён (в каждом кадре)
  std::erase_if(src_list,
    [this](Str src)->bool {
      auto filter_str = from_null_ended<Str>(filter);
      src = str_tolower(src);
      filter_str = str_tolower(filter_str);
      return !(src.find(filter_str) != Str::npos);
    }
  );

  auto list = strs_to_null_terminated(src_list);
  int idx = get_cur_list_idx(sel_spr, src_list);
  
  if (Combo("##resources", &idx, list.data()) )
    sel_spr = src_list.at(idx);
    
  if (Button("select"))
    enable();

  SameLine();
  if (Button("exit"))
    hpw::scene_mgr->back();

  PopItemWidth();
} // imgui_execute

void Sprite_select::enable() {
  return_if(sel_spr.empty());

  if (callback) {
    auto path = sel_spr;
    auto sprite = hpw::store_sprite->find(path);
    callback(sprite, path);
  }
  hpw::scene_mgr->back();
}

Strs Sprite_select::get_list() const {
  return _list;
}

int Sprite_select::get_cur_list_idx(CN<Str> name, CN<Strs> list) const {
  for (int idx = 0; cnauto str: list) {
    if (str == name)
      return idx;
    ++idx;
  }
  return 0;
}