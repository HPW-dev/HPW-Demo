#include <ranges>
#include <utility>
#include <iostream>
#include <ctime>
#include <algorithm>
#include <unordered_map>
#include <cassert>
#include <filesystem>
#include <sstream>
#include "store.hpp"
#include "hash_sha256/hash_sha256.h"
#include "game/util/game-util.hpp"
#include "game/util/game-archive.hpp"
#include "game/util/locale.hpp"
#include "game/util/keybits.hpp"
#include "game/util/config.hpp"
#include "game/core/anims.hpp"
#include "game/core/locales.hpp"
#include "game/core/user.hpp"
#include "game/core/scenes.hpp"
#include "game/core/common.hpp"
#include "game/core/core.hpp"
#include "game/core/canvas.hpp"
#include "game/core/fonts.hpp"
#include "game/core/sounds.hpp"
#include "game/core/palette.hpp"
#include "game/core/graphic.hpp"
#include "game/scene/scene-mgr.hpp"
#include "util/file/yaml.hpp"
#include "util/path.hpp"
#include "util/hpw-util.hpp"
#include "util/log.hpp"
#include "util/error.hpp"
#include "util/rnd-table.hpp"
#include "util/platform.hpp"
#include "util/math/circle.hpp"
#include "util/math/polygon.hpp"
#include "util/math/vec-util.hpp"
#include "util/math/random.hpp"
#include "graphic/util/graphic-util.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/sprite/sprite-io.hpp"
#include "graphic/image/image-io.hpp"
#include "graphic/image/image.hpp"
#include "graphic/animation/anim-io.hpp"
#include "graphic/animation/anim.hpp"
#include "graphic/animation/frame.hpp"
#include "graphic/font/unifont.hpp"
#include "graphic/font/unifont-mono.hpp"
#include "graphic/effect/blur.hpp"
#include "sound/sound-mgr.hpp"
#include "sound/audio-io.hpp"
#include "sound/sound.hpp"
#include "host/command.hpp"

// грузит спрайт либо из файловой системы, либо из архива
Shared<Sprite> sprite_loader(cr<Str> name) {
  assert(hpw::archive);

  try {
    auto spr = new_shared<Sprite>();
    #ifdef EDITOR
      load(*spr, name);
    #else
      load(hpw::archive->get_file(name), *spr);
    #endif
    return spr;
  } catch (...) {
    detailed_log("не удалось найти спрайт с имененм \"" << name << "\"\n");
  }
  
  return {};
}

decltype(hpw::sprites)::Velue find_err_cb(cr<Str> _name) {
  auto name {_name};
  #ifdef EDITOR
    name = hpw::cur_dir + "../" + name;
  #endif
  cauto spr = sprite_loader(name);
  detailed_iflog(!spr, "sprite \"" << name << "\" not finded\n");
  // закинуть недостающий ресурс в банк
  if (spr)
    hpw::sprites.push(name, spr);
  return spr;
}

void load_resources() {
  hpw_log("загрузка ресурсов...\n");
  hpw::sprites.clear();

  #ifdef EDITOR
    auto names = all_names_in_dir(hpw::cur_dir + "../");
  #else
    auto names = hpw::archive->get_all_names();
  #endif

  // колбек на отложенную загрузку
  if (hpw::lazy_load_sprite) {
    hpw::sprites.move_find_err_cb(&find_err_cb);
    return;
  } // make fined err cb

  // загрузка всех спрайтов
  // фильтр пропускает только файлы в нужной папке и с нужным разрешением
  auto name_filter = [](cr<Str> name) {
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
    auto sprite = sprite_loader(name);
    #ifdef EDITOR
      delete_all(name, hpw::cur_dir + ".." + SEPARATOR);
      conv_sep_for_archive(name);
    #endif
    hpw::sprites.push(name, sprite);
  }
}

void load_animations() {
  init_unique(hpw::anim_mgr);

  if (!hpw::lazy_load_anim) {
    cauto anim_yml = get_anim_config();
    read_anims(anim_yml);
  }
}

cr<utf32> get_locale_str(cr<Str> key) {
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

Circle cover_polygons(cr<Vector<Polygon>> polygons) {
  if (polygons.empty())
    return {};

  // найти крайние точки
  Vec point_min(99999, 99999);
  Vec point_max(-1, -1);
  for (crauto poly: polygons)
  for (crauto point: poly.points) {
    point_min.x = std::min(point_min.x, point.x + poly.offset.x);
    point_min.y = std::min(point_min.y, point.y + poly.offset.y);
    point_max.x = std::max(point_max.x, point.x + poly.offset.x);
    point_max.y = std::max(point_max.y, point.y + poly.offset.y);
  }
  // определить центр
  Vec mid = point_min + (point_max - point_min) * 0.5;
  // найти самую дальнюю точку от центра
  real dist = 0;
  for (crauto poly: polygons)
  for (crauto point: poly.points)
    dist = std::max(dist, distance(mid, point + poly.offset));
  return Circle(mid, dist);
} // cover_polygons

Vec get_screen_center() { return Vec(graphic::width / 2.0, graphic::height / 2.0); }

// создаёт спрайт только с белыми контурами по исходному спрайту
Sprite extract_contour(cr<Sprite> src) {
  if (!src) {
    hpw_log("WARNING: extract_contour src is empty\n");
    return {};
  }
  // расширение спрайта во все стороны на 1 пиксель (нужна только маска)
  Image ext_mask(src.X() + 2, src.Y() + 2, Pal8::mask_invisible);
  // вставить целевой спрайт в центр расширенного
  insert(ext_mask, src.mask(), Vec(1, 1));

  Sprite ret (ext_mask.X, ext_mask.Y);
  // определить контуры
  for (int y = 1; y < ext_mask.Y - 1; ++y)
  for (int x = 1; x < ext_mask.X - 1; ++x) {
    cauto mask_pix = ext_mask(x, y);
    cont_if (mask_pix == Pal8::mask_invisible);

    #define set_white(x0, y0) if (ext_mask(x0, y0) == Pal8::mask_invisible) { \
      ret.image().fast_set(x0, y0, Pal8::white, {}); \
      ret.mask().fast_set(x0, y0, Pal8::mask_visible, {}); \
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

cr<Shared<Anim>> make_light_mask(cr<Str> src, cr<Str> dst) {
  auto dst_anim = new_shared<Anim>();
  auto src_anim = hpw::anim_mgr->find_anim(src);
  assert(src_anim);

  *dst_anim = *src_anim;
  dst_anim->set_name(dst);
  // замена рисунка спрайта на обведёные контуры
  for (crauto frames: dst_anim->get_frames()) {
    for (crauto _direct: frames->get_directions()) {
      // допустимое преобразование:
      rauto direct = ccast<Direct&>(_direct);
      crauto sprite_for_contour = *direct.sprite.lock();

      direct.offset += -Vec(1, 1);
      direct.sprite =hpw::sprites.push (
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
  
  auto player_name = utf32_to_8(hpw::player_name);
  // укоротить имя игрока, если оно большое
  if (player_name.size() > 50) {
    player_name = player_name.substr(0, 50) + "...";
  }
  name << player_name;

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

Str calc_sum(cp<void> data, std::size_t sz) {
  hash_sha256 hash;
  hash.sha256_init();
  hash.sha256_update(scast<cp<uint8_t>>(data), sz);
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
  hpw::exe_sha256 = calc_sum( scast<cp<void>>(mem.data()), mem.size() );

  // DATA
  path = hpw::cur_dir + hpw::data_path;
  mem = mem_from_file(path);
  hpw::data_sha256 = calc_sum( scast<cp<void>>(mem.data()), mem.size() );

  hpw_log("game executable SHA256: " + hpw::exe_sha256 + "\n");
  hpw_log("game data.zip SHA256: " + hpw::data_sha256 + "\n");
} // init_validation_info

void init_scene_mgr() { init_unique(hpw::scene_mgr); }

utf32 difficulty_to_str(const Difficulty difficulty) {
  static const std::unordered_map<Difficulty, utf32> table {
    {Difficulty::easy, get_locale_str("scene.difficulty_select.difficulty.easy")},
    {Difficulty::normal, get_locale_str("scene.difficulty_select.difficulty.normal")},
    {Difficulty::hardcore, get_locale_str("scene.difficulty_select.difficulty.hardcore")},
  };
  return table.at(difficulty);
}

/** нарезает ресурсы на атлас для save_all_sprites
*@return если вернул true, то можно продолжать нарезку ресурсов */
inline static bool stream_concat(cr<Strs> sprite_list,
std::size_t& idx, Sprite& buffer) {
  assert(!sprite_list.empty());
  assert(buffer);
  uint timeout = 100'000;
  int pos_x {}, pos_y {};
  /* чтобы следующая строка спрайта не была на уровне меньше,
  чем максимальная высота спрайтов на уровне выше */
  int max_pos_y {}; 

  while (idx < sprite_list.size()) {
    // чтобы не уйти в вечный цикл
    if (timeout == 0)
      break;
    else
      --timeout;

    cauto sprite = hpw::sprites.find(sprite_list.at(idx));
    ++idx;
    cont_if (!sprite || !(*sprite));

    // скипнуть слишком большие спрайты
    cont_if(sprite->size() >= 256*256);

    max_pos_y = std::max(max_pos_y, sprite->Y());

    // найти свободное место для вставки
    if (pos_x + sprite->X() >= buffer.X()) {
      pos_x = 0;
      pos_y += max_pos_y;
      max_pos_y = 0;
    }
    // если места не нашлось, оставить вставку другим вызовам этой функции
    if (pos_y + sprite->Y() >= buffer.Y()) {
      idx = std::max<int>(idx - 1, 0); // отменить смену спрайта
      break;
    }
    insert(buffer.image(), sprite->image(), Vec(pos_x, pos_y));
    insert(buffer.mask(), sprite->mask(), Vec(pos_x, pos_y));
    pos_x += sprite->X();
  }
  return idx < sprite_list.size();
}

// фильтрует и сортирует имена спрайтов для save_all_sprites
inline static void prepare_sprite_list(Strs& sprite_list) {
  constexpr auto name_filter = [](cr<Str> sprite_name) {
    cauto sprite = hpw::sprites.find(sprite_name);
    const bool exist = sprite && *sprite;
    return sprite_name.find("tile") == str_npos // тайлы уровня не нужны
      && sprite_name.find("contour") == str_npos // контуры не нужны
      && sprite_name.find("logo") == str_npos // логотипы не нужны
      && sprite_name.find("palette") == str_npos // палитры не нужны
      && exist; // картинка есть
  };
  Strs tmp;
  to_vector(tmp, sprite_list | std::views::filter(name_filter));
  sprite_list = tmp;

  // сортировка спрайтов по размеру
  std::sort(sprite_list.begin(), sprite_list.end(), [](cr<Str> a, cr<Str> b) {
    cauto sprite_a = hpw::sprites.find(a);
    cauto sprite_b = hpw::sprites.find(b);
    // при одинаковых размерах сортировать по ширине
    if (sprite_a->size() == sprite_b->size())
      return sprite_a->X() > sprite_b->X();
    return sprite_a->size() > sprite_b->size();
  });
} // prepare_sprite_list

void save_all_sprites(cr<Str> save_dir, const int MX, const int MY) {
  assert(MX >= 256);
  assert(MY >= 256);
  make_dir_if_not_exist(save_dir);
  auto sprite_list = hpw::sprites.list();
  prepare_sprite_list(sprite_list);
  std::size_t idx {};
  uint time_out = 1'000;
  
  while (true) {
    // чтобы не уйти в вечный цикл
    if (time_out == 0)
      break;
    else
      --time_out;

    Sprite buffer(MX, MY);
    cauto continue_stream = stream_concat(sprite_list, idx, buffer);
    save(buffer, save_dir + "sprite pack " + n2s(idx) + ".png");
    break_if(!continue_stream);
  }
}

std::size_t sizeof_all_sprites() {
  return_if (hpw::sprites.empty(), 0);
  std::size_t ret {};
  cauto sprite_list = hpw::sprites.list();
  for (crauto sprite_name: sprite_list) {
    cauto sprite = hpw::sprites.find(sprite_name);
    if (sprite) {
      ret += sizeof(Sprite); // размер класса под спрайт
      if (*sprite) // размер пикселей в маске и картинке
        ret += sprite->size() * 2 * sizeof(Pal8);
    }
  }
  return ret;
}

// связывание звуков с банком
inline void init_store_sound() {
  init_unique(hpw::store_sound);
  // TODO
  // загрузка контента
}

void load_sounds() {
  hpw_log("загрузка звуков...\n");

  try {
    // TODO применение настроек при создании
    init_unique<Sound_mgr_oal>(hpw::sound_mgr);
  } catch (cr<hpw::Error> err) {
    hpw_log("Error while initialize OpenAL sound system. Sound disabled\n");
    std::cerr << err.what() << std::endl;
    hpw::sound_mgr_init_error = true;
    init_unique<Sound_mgr_nosound>(hpw::sound_mgr);
  }
  hpw::sound_mgr->set_doppler_factor(hpw::DEFAULT_DOPPLER_FACTOR);
  
#ifdef EDITOR
  auto names = all_names_in_dir(hpw::cur_dir + "../");
#else
  auto names = hpw::archive->get_all_names();
#endif
  // фильтр пропускает только файлы в нужной папке и с нужным разрешением
  auto name_filter = [](cr<Str> name) {
    Str find_str = "resource/audio/";
#ifdef EDITOR
    conv_sep(find_str);
#endif
    return name.find(find_str) != str_npos &&
      !std::filesystem::path(name).extension().empty() && // не директория
      ( // подходит формат
        std::filesystem::path(name).extension() == ".ogg" ||
        std::filesystem::path(name).extension() == ".mp3" ||
        std::filesystem::path(name).extension() == ".opus" ||
        std::filesystem::path(name).extension() == ".flac"
      );
  };
  Strs file_names;
  to_vector(file_names, names | std::views::filter(name_filter));
  iferror (file_names.empty(), "file_names пуст, возможно нет ресурсов в папках");
  // загрузка в хранилище
  for (auto &name: file_names) {
#ifdef EDITOR
    auto sound = load_audio(name);
    delete_all(name, hpw::cur_dir + ".." + SEPARATOR);
    conv_sep_for_archive(name);
#else
    auto sound = load_audio_from_memory(hpw::archive->get_file(name));
#endif
    delete_all(name, "resource/audio/");
    hpw::sound_mgr->move_audio(name, std::move(sound));
  }

  init_store_sound();
} // load_sounds

[[nodiscard]] Vec rnd_screen_pos_safe() {
  assert(graphic::canvas);
  return Vec {
    rndr(0, graphic::canvas->X),
    rndr(0, graphic::canvas->Y)
  };
}

[[nodiscard]] Vec rnd_screen_pos_fast() {
  assert(graphic::canvas);
  return Vec {
    rndr_fast(0, graphic::canvas->X),
    rndr_fast(0, graphic::canvas->Y)
  };
}

void set_random_palette() {
  cauto sprites = hpw::archive->get_all_names(false);;
  cauto filter = [](cr<Str> src) {
    return src.find("resource/image/palettes/") != Str::npos;
  };
  Rnd_table<Str> palettes( sprites | std::views::filter(filter)
    | std::ranges::to<Strs>() );
  cauto palette_name = palettes.rnd_stable();
  graphic::current_palette_file = palette_name;
  hpw::init_palette_from_archive (palette_name);
}

void save_screenshot(cr<Image> image) {
  assert(image);

  auto t = std::time(nullptr);
#ifdef LINUX
  struct ::tm lt;
  ::localtime_r(&t, &lt);
#else // WINDOWS
  auto lt = *std::localtime(&t);
#endif

  cauto screenshots_dir = hpw::cur_dir + 
    (*hpw::config)["path"].get_str("screenshots", hpw::screenshots_path) + SEPARATOR;
  std::ostringstream oss;
  oss << screenshots_dir;
  make_dir_if_not_exist(oss.str());
  oss << std::put_time(&lt, "%d-%m-%Y %H-%M-%S");
  oss << "-" + n2s(rndu_fast(100'000)) + ".png";
  save(image, oss.str());
} // save_screenshot

void load_locale(cr<Str> user_path) {
  hpw_log("загрузка локализации...\n");
  assert(hpw::config);
  File mem; 
  cauto path = user_path.empty()
    ? (*hpw::config)["path"].get_str("locale", hpw::fallback_locale_path)
    : user_path;

  try {
    mem = hpw::archive->get_file(path);
  } catch (...) {
    hpw_log("ошибка при загрузке перевода \"" << path << "\". Попытка загрузить перевод \""
      << hpw::fallback_locale_path << "\"\n");
    mem = hpw::archive->get_file(hpw::fallback_locale_path);
  }

  hpw::locale_path = mem.get_path();
  auto yml = Yaml(mem);
  load_locales_to_store(yml);
}

void load_fonts() {
  hpw_log("загрузка шрифтов...\n");
  assert(hpw::archive);
  auto mem = hpw::archive->get_file("resource/font/unifont-13.0.06.ttf");
  init_unique<Unifont>(graphic::font, mem, 16, true);
  init_unique<Unifont_mono>(graphic::system_mono, mem, 8, 16, true);
}

void hpw_blur(Image& dst, cr<Image> src, const int window_sz) {
  switch (graphic::blur_mode) {
    default:
    case Blur_mode::autoopt: {
      if (graphic::render_lag)
        blur_fast(dst, src, window_sz);
      else
        blur_hq(dst, src, window_sz);
      break;
    }
    case Blur_mode::low: blur_fast(dst, src, window_sz); break;
    case Blur_mode::high: blur_hq(dst, src, window_sz); break;
  }
}

bool check_high_blur() {
  switch (graphic::blur_mode) {
    default:
    case Blur_mode::autoopt: return !graphic::render_lag;
    case Blur_mode::low: return false;
    case Blur_mode::high: return true;
  }
  return false;
}
