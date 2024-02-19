#include <ranges>
#include <ctime>
#include <algorithm>
#include <cassert>
#include <filesystem>
#include <sstream>
#include "store.hpp"
#include "hash_sha256/hash_sha256.h"
#include "game/util/game-util.hpp"
#include "game/util/game-archive.hpp"
#include "game/util/locale.hpp"
#include "game/util/keybits.hpp"
#include "game/scene/scene-manager.hpp"
#include "game/game-common.hpp"
#include "game/game-core.hpp"
#include "game/game-canvas.hpp"
#include "game/game-font.hpp"
#include "util/file/archive.hpp"
#include "util/file/yaml.hpp"
#include "util/hpw-util.hpp"
#include "util/log.hpp"
#include "util/math/circle.hpp"
#include "util/math/polygon.hpp"
#include "util/math/vec-util.hpp"
#include "util/math/random.hpp"
#include "graphic/util/graphic-util.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/sprite/sprite-io.hpp"
#include "graphic/sprite/sprite.hpp"
#include "graphic/font/font.hpp"
#include "graphic/image/image.hpp"
#include "graphic/animation/animation-manager.hpp"
#include "graphic/animation/anim-io.hpp"
#include "graphic/animation/anim.hpp"
#include "graphic/animation/frame.hpp"

void load_resources() {
  detailed_log("loading resources...\n");
  init_store_sprite();
#ifdef EDITOR
  auto names = all_names_in_dir(hpw::cur_dir);
#else
  auto names = hpw::archive->get_all_names();
#endif
// загрузка спрайтов из файлов системы
  // фильтр пропускает только файлы в нужной папке и с нужным разрешением
  auto name_filter = [](CN<Str> name) {
    Str find_str = "resource/image/";
#ifdef EDITOR
    conv_sep(find_str);
#endif
    return name.find(find_str) != str_npos &&
      !std::filesystem::path(name).extension().empty() && // не директория
      ( // подходит формат
        std::filesystem::path(name).extension() == ".png" ||
        std::filesystem::path(name).extension() == ".webp"
      );
  };
  Strs image_names;
  to_vector(image_names, names | std::views::filter(name_filter));
  iferror (image_names.empty(), "image_names пуст, возможно нет ресурсов в папках");
  // загрузка в хранилище
  for (auto &name: image_names) {
    auto spr {new_shared<Sprite>()};
#ifdef EDITOR
    load(*spr, name);
    delete_all(name, hpw::cur_dir);
    conv_sep_for_archive(name);
#else
    load(hpw::archive->get_file(name), *spr);
#endif
    hpw::store_sprite->push(name, spr);
  }
} // load_resources

void load_animations() {
  init_anim_mgr();
  Shared<Yaml> anim_yml;
#ifdef EDITOR
  anim_yml = new_shared<Yaml>(hpw::cur_dir + "config/animation.yml");
#else
  anim_yml = new_shared<Yaml>(hpw::archive->get_file("config/animation.yml"));
#endif
  read_anims(*anim_yml);
} // load_animations

CN<utf32> get_locale_str(CN<Str> key) {
  assert(hpw::store_locale);
  if (auto ret = hpw::store_locale->find(key); ret)
    return ret->str;
  else
    detailed_log("not found string: \"" << key << "\"\n");
  static utf32 last_error;
  detailed_log("not finded string \"" << key << "\"\n");
  last_error = U"_ERR_(" + sconv<utf32>(key) + U")";
  return last_error;
}

Circle cover_polygons(CN<Vector<Polygon>> polygons) {
  if (polygons.empty())
    return {};

  // найти крайние точки
  Vec point_min(99999, 99999);
  Vec point_max(-1, -1);
  for (cnauto poly: polygons)
  for (cnauto point: poly.points) {
    point_min.x = std::min(point_min.x, point.x + poly.offset.x);
    point_min.y = std::min(point_min.y, point.y + poly.offset.y);
    point_max.x = std::max(point_max.x, point.x + poly.offset.x);
    point_max.y = std::max(point_max.y, point.y + poly.offset.y);
  }
  // определить центр
  Vec mid = point_min + (point_max - point_min) * 0.5;
  // найти самую дальнюю точку от центра
  real dist = 0;
  for (cnauto poly: polygons)
  for (cnauto point: poly.points)
    dist = std::max(dist, distance(mid, point + poly.offset));
  return Circle(mid, dist);
} // cover_polygons

Vec get_screen_center() { return Vec(graphic::width / 2.0, graphic::height / 2.0); }

/// создаёт спрайт только с белыми контурами по исходному спрайту
Sprite extract_contour(CN<Sprite> src) {
  // расширение спрайта во все стороны на 1 пиксель (нужна только маска)
  Image ext_mask(src.X() + 2, src.Y() + 2, Pal8::mask_invisible);
  // вставить целевой спрайт в центр расширенного
  insert(ext_mask, *src.get_mask(), Vec(1, 1));

  Sprite ret (ext_mask.X, ext_mask.Y);
  // определить контуры
  for (int y = 1; y < ext_mask.Y - 1; ++y)
  for (int x = 1; x < ext_mask.X - 1; ++x) {
    cauto mask_pix = ext_mask(x, y);
    cont_if (mask_pix == Pal8::mask_invisible);

    #define set_white(x0, y0) if (ext_mask(x0, y0) == Pal8::mask_invisible) { \
      ret.get_image()->fast_set(x0, y0, Pal8::white, {}); \
      ret.get_mask()->fast_set(x0, y0, Pal8::mask_visible, {}); \
    }
    set_white(x+1, y+0);
    set_white(x-1, y+0);
    set_white(x+0, y-1);
    set_white(x+0, y+1);
    #undef set_white
  } // for x/y

  ret.set_generated(true);
  ret.set_path(src.get_path() + ".extended");
  return ret;
} // extract_contour

CN<Shared<Anim>> make_light_mask(CN<Str> src, CN<Str> dst) {

  auto dst_anim = new_shared<Anim>();
  auto src_anim = hpw::anim_mgr->find_anim(src);
  assert(src_anim);

  *dst_anim = *src_anim;
  dst_anim->set_name(dst);
  // замена рисунка спрайта на обведёные контуры
  for (cnauto frames: dst_anim->get_frames()) {
    for (cnauto _direct: frames->get_directions()) {
      // допустимое преобразование:
      nauto direct = ccast<Direct&>(_direct);
      cnauto sprite_for_contour = *direct.sprite.lock();

      direct.offset += -Vec(1, 1);
      direct.sprite =hpw::store_sprite->push (
        sprite_for_contour.get_path() + ".contour",
        new_shared<Sprite>(extract_contour(sprite_for_contour))
      );
    } // for directs
  } // for frames

  // закинуть новую анимацию в банк
  hpw::anim_mgr->add_anim(dst, dst_anim);
  return hpw::anim_mgr->find_anim(dst);
  
} // make_light_mask

Str get_random_replay_name() {
  std::stringstream name;
  
  // TODO + player name
  name << "Rename";

  auto t = std::time(nullptr);
  #ifdef LINUX
    struct ::tm lt;
    ::localtime_r(&t, &lt);
  #else // WINDOWS
    auto lt = *std::localtime(&t);
  #endif
  name << " @ " << std::put_time(&lt, "Date %d-%m-%Y @ Time %H-%M-%S");

  std::stringstream rnd_num_str;
  rnd_num_str << std::hex << rndu_fast();
  name << " @ UID " << str_toupper(rnd_num_str.str());

  name << ".hpw_replay";
  return name.str();
}

Vec get_rand_pos_safe(const real sx, const real sy, const real ex, const real ey) {
  return Vec(
    rndr(sx, ex),
    rndr(sy, ey)
  );
}

Vec get_rand_pos_graphic(const real sx, const real sy, const real ex, const real ey) {
  return Vec(
    rndr_fast(sx, ex),
    rndr_fast(sy, ey)
  );
}

void draw_controls(Image& dst) {
  const Vec pos (5, 300);
  const Vec text_offset (5, 5);
  draw_rect_filled<&blend_158>(dst, Rect(pos.x, pos.y, 65, 55), Pal8::black);

  utf32 inputs;
  inputs += is_pressed(hpw::keycode::mode)  ? U"M" : U"_";
  inputs += is_pressed(hpw::keycode::shoot) ? U"S" : U"_";
  inputs += U"  ";
  inputs += is_pressed(hpw::keycode::up)    ? U"#" : U"_";
  inputs += U" \n";

  inputs += U" ";
  inputs += is_pressed(hpw::keycode::bomb)  ? U"B" : U"_";
  inputs += U" ";
  inputs += is_pressed(hpw::keycode::left)  ? U"#" : U"_";
  inputs += is_pressed(hpw::keycode::down)  ? U"#" : U"_";
  inputs += is_pressed(hpw::keycode::right) ? U"#" : U"_";
  inputs += U" \n";

  inputs += U" ";
  inputs += is_pressed(hpw::keycode::focus) ? U"F" : U"_";
  graphic::font->draw(dst, pos + text_offset, inputs);
} // draw_controls

Str calc_sum(CP<void> data, std::size_t sz) {
  hash_sha256 hash;
  hash.sha256_init();
  hash.sha256_update(scast<CP<uint8_t>>(data), sz);
  auto hash_ret = hash.sha256_final();
        
  std::stringstream ss;
  for (auto val: hash_ret)
    ss << std::hex << int(val);
  return str_toupper(ss.str());
}

void init_validation_info() {
  // EXE
  #ifdef WINDOWS
    Str path = hpw::cur_dir + "HPW.exe";
  #else
    Str path = hpw::cur_dir + "HPW";
  #endif
  auto mem = mem_from_file(path);
  hpw::exe_sha256 = calc_sum( scast<CP<void>>(mem.data()), mem.size() );

  // DATA
  path = hpw::cur_dir + "data.zip";
  mem = mem_from_file(path);
  hpw::data_sha256 = calc_sum( scast<CP<void>>(mem.data()), mem.size() );

  hpw_log("game executable SHA256: " + hpw::exe_sha256 + "\n");
  hpw_log("game data.zip SHA256: " + hpw::data_sha256 + "\n");
} // init_validation_info
