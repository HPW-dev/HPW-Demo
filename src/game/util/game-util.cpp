#include <ranges>
#include <utility>
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

void load_animations() {
  init_unique(hpw::anim_mgr);

  if (!hpw::lazy_load_anim) {
    cauto anim_yml = get_anim_config();
    read_anims(anim_yml);
  }
}

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
    hpw_log("Error while initialize OpenAL sound system. Sound disabled\n", Log_stream::warning);
    hpw_log(Str("Details: ") + err.what() + '\n', Log_stream::warning);
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

void load_fonts() {
  hpw_log("загрузка шрифтов...\n");
  assert(hpw::archive);
  auto mem = hpw::archive->get_file("resource/font/unifont-13.0.06.ttf");
  init_unique<Unifont>(graphic::font, mem, 16, true);
  init_unique<Unifont>(graphic::font_shop, mem, 32, true);
  init_unique<Unifont_mono>(graphic::system_mono, mem, 8, 16, true);
}
