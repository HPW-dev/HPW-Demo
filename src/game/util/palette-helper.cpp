#include <cassert>
#include <ranges>
#include "palette-helper.hpp"
#include "graphic/image/palette.hpp"
#include "graphic/image/color.hpp"
#include "graphic/image/color-convert.hpp"
#include "game/core/palette.hpp"
#include "game/menu/item/list-item.hpp"
#include "game/util/resource-helper.hpp"
#include "game/util/locale.hpp"
#include "host/command.hpp"
#include "util/file/file.hpp"
#include "util/str.hpp"
#include "util/path.hpp"
#include "util/rnd-table.hpp"

void randomize_palette() {
  assert(hpw::init_palette_from_archive);

  cauto sprites = get_all_res_names(false);
  cauto filter = [](cr<Str> src)
    { return src.find("resource/image/palettes/") != Str::npos; };
  Rnd_table<Str> palettes(sprites | std::views::filter(filter) | std::ranges::to<Strs>());
  cauto palette_name = palettes.rnd_stable();
  graphic::current_palette_file = palette_name;
  hpw::init_palette_from_archive(palette_name);
}

struct Palette_file_name {
  Str full_path {};
  Str short_name {};
};
using Palette_file_names = Vector<Palette_file_name>;

// загрузить имена всех файлов с палитрой
inline static Palette_file_names get_palette_file_names() {
  Palette_file_names ret;

  auto file_list = get_all_res_names(false);
  // фильтр списка
  std::erase_if(file_list, [](cr<Str> src) {
    return src.find(graphic::DEFAULT_PALETTE_FILES_DIR) == str_npos
      || src == graphic::DEFAULT_PALETTE_FILES_DIR;
  });
  
  // сортирует список палитр по цветам
  std::sort(file_list.begin(), file_list.end(), [](cr<Str> a, cr<Str> b) {
    return by_similar(
      load_palette(load_res(a)),
      load_palette(load_res(b))
    );
  });
    
  for (crauto palette_name: file_list) {
    ret.push_back(Palette_file_name {
      .full_path = palette_name,
      .short_name = get_filename(palette_name)
    });
  }
  return ret;
}

// указать какае изначально выбрана палитра
inline static std::size_t get_default_item_id() {
  cauto palette_files = get_palette_file_names();
  
  for (std::size_t ret = 0; crauto palette_file: palette_files) {
    if (palette_file.full_path == graphic::current_palette_file)
      return ret;
    ++ret;
  }

  return 0;
}

Shared<Menu_list_item> get_palette_list() {
  cauto palette_files = get_palette_file_names();
  Menu_list_item::Items items;
  // каждый элемент списка применяет свою палитру:
  for (crauto palette_file: palette_files) {
    items.push_back( Menu_list_item::Item {
      .name = utf8_to_32(palette_file.short_name),
      .desc = {},
      .action = [path = palette_file.full_path] {
        assert(hpw::init_palette_from_archive);
        graphic::current_palette_file = path;
        hpw::init_palette_from_archive(graphic::current_palette_file);
      }
    } );
  }

  return new_shared<Menu_list_item>(get_locale_str("palette_select.cur_file"), items, &get_default_item_id);
}
