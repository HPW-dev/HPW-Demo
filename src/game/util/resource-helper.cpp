#include <ranges>
#include <cassert>
#include <filesystem>
#include "resource-helper.hpp"
#include "graphic/sprite/sprite-io.hpp"
#include "graphic/util/util-templ.hpp"
#include "util/log.hpp"
#include "util/error.hpp"
#include "util/path.hpp"
#include "util/file/file-io.hpp"
#include "util/str-util.hpp"
#include "game/core/sprites.hpp"
#include "game/core/common.hpp"
#include "game/util/game-archive.hpp"
#include "game/util/resource-helper.hpp"

// грузит спрайт либо из файловой системы, либо из архива
static inline Shared<Sprite> sprite_loader(cr<Str> name) {
  try {
    auto spr = new_shared<Sprite>();
    #ifdef EDITOR
      load(*spr, name);
    #else
      load(load_res(name), *spr);
    #endif
    return spr;
  } catch (...) {
    hpw_log("не удалось найти спрайт с имененм \"" + name + "\"\n", Log_stream::debug);
  }
  
  return {};
}

static inline decltype(hpw::sprites)::Velue find_err_cb(cr<Str> _name) {
  auto name {_name};
  #ifdef EDITOR
    name = hpw::cur_dir + hpw::os_resources_dir + name;
  #endif
  cauto spr = sprite_loader(name);
  if (!spr)
    hpw_log("sprite \"" + name + "\" not finded\n", Log_stream::debug);
  // закинуть недостающий ресурс в банк
  if (spr)
    hpw::sprites.push(name, spr);
  return spr;
}

void load_resources() {
  hpw_log("загрузка ресурсов...\n");
  hpw::sprites.clear();

  #ifdef EDITOR
    auto names = all_names_in_dir(hpw::cur_dir + hpw::os_resources_dir);
  #else
    auto names = get_all_res_names();
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
  iferror(image_names.empty(), "image_names пуст, возможно нет ресурсов в папках");

  // загрузка в хранилище
  for (auto &name: image_names) {
    #ifdef EDITOR
    name = std::filesystem::absolute(name).string();
    cauto path_for_delete = std::filesystem::absolute(hpw::cur_dir + hpw::os_resources_dir + SEPARATOR).string();
    #endif

    auto sprite = sprite_loader(name);

    #ifdef EDITOR
    delete_all(name, path_for_delete);
    conv_sep_for_archive(name);
    #endif

    hpw::sprites.push(name, sprite);
  }
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

File load_res(cr<Str> name) {
  assert(!name.empty());

  // попытка загрузить ресурс с архива:
  try {
    iferror(!hpw::archive, "hpw::archive не инициализирован\n");
    return hpw::archive->get_file(name);
  } catch(cr<hpw::Error> err) {
    hpw_log(Str("Ошибка при загрузке ресурса \"") + name + "\" из архива\n" + err.what(), Log_stream::debug);
  } catch(...) {
    hpw_log(Str("не удалось загрузить ресурс \"") + name + "\" из архива\n", Log_stream::debug);
  }

  // попытка загрузить ресурс из файловой ситсемы ОС:
  auto os_path = hpw::cur_dir + hpw::os_resources_dir + name;
  conv_sep(os_path);
  return file_load(os_path);
}

Strs get_all_res_names(const bool with_folders) {
  return_if (hpw::archive, hpw::archive->get_all_names(with_folders));

  hpw_log("не удалось получить все имена ресурсов из архива.\n"
    "Попытка получить их из ОС...\n", Log_stream::debug);

  auto ret = files_in_dir(hpw::cur_dir + hpw::os_resources_dir, true);
  for (rauto fname: ret)
    conv_sep_for_archive(fname);
  // вырезать имена папок
  if (!with_folders) {
    std::erase_if(ret, [](cr<Str> path) {
      return_if (path.empty(), true);
      return path.back() == '/';
    });
  }
  // вырезать базовую директорию
  auto path_for_cut = hpw::cur_dir + hpw::os_resources_dir;
  conv_sep_for_archive(path_for_cut);
  for (rauto fname: ret) {
    delete_all(fname, path_for_cut);
  }
  return ret;
}

Str to_res_path(Str src) {
  // преобразовать в пути с / и вырезать рутовый адрес
  conv_sep_for_archive(src);
  auto base_dir = hpw::cur_dir + hpw::os_resources_dir;
  conv_sep_for_archive(base_dir);
  delete_all(src, base_dir);
  return src;
}
