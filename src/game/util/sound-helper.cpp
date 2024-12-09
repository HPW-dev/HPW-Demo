#include <cassert>
#include <filesystem>
#include <ranges>
#include "sound-helper.hpp"
#include "util/log.hpp"
#include "util/error.hpp"
#include "util/str-util.hpp"
#include "game/util/resource-helper.hpp"
#include "game/core/common.hpp"
#include "game/core/sounds.hpp"
#include "sound/audio-io.hpp"

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
  auto names = get_all_res_names();

  // фильтр пропускает только файлы в нужной папке и с нужным разрешением
  auto name_filter = [](cr<Str> name) {
    Str find_str = "resource/audio/";

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
      auto sound = load_audio_from_memory(load_res(name));
    #endif
    delete_all(name, "resource/audio/");
    hpw::sound_mgr->move_audio(name, std::move(sound));
  }

  init_store_sound();
}
