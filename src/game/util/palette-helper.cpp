#include <cassert>
#include <ranges>
#include "palette-helper.hpp"
#include "graphic/image/palette.hpp"
#include "graphic/image/color.hpp"
#include "game/core/palette.hpp"
#include "game/util/resource-helper.hpp"
#include "menu/item/list-item.hpp"
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

inline static real rgb24_to_hue(cr<Rgb24> src) {
  const real r = src.r / 255.f;
  const real g = src.g / 255.f;
  const real b = src.b / 255.f;
  real min = r < g ? r : g;
  min = min < b ? min : b;
  real max = r > g ? r : g;
  max = max > b ? max : b;
  cauto delta = max - min;

  // undefined, maybe nan?
  return_if (delta < 0.00001f, 0);
  return_if(max == 0, 0);

  real hue {};
  if(r >= max) // > is bogus, just keeps compilor happy
    return (g - b) / delta; // between yellow & magenta
  elif (g >= max)
    hue = 2.0 + (b - r) / delta; // between cyan & yellow
  else
    hue = 4.0 + (r - g) / delta; // between magenta & cyan
  hue *= 60.0; // degrees

  if(hue < 0)
    hue += 360;
  return hue;
}

// создаёт код для сравнения цветовых палитр
inline static uint pal24_score(cr<Vector<Rgb24>> pal24) {
  assert(pal24.size() >= 256);
  // взять часть серого сектора и вычислить среднюю цветность
  const std::size_t ED = Pal8::gray_end * 0.9;
  const std::size_t ST = Pal8::gray_end * 0.4;
  cauto LEN = ED - ST;
  real avg_hue {};
  for (std::size_t i = ST; i < ED; ++i) {
    cauto rgb24 = pal24.at(i);
    avg_hue += rgb24_to_hue(rgb24);
  }
  avg_hue /= LEN;

  // средняя яркость конца палитры тоже повлияет на результат
  cauto a = pal24.at(ED * 0.85);
  cauto b = pal24.at(Pal8::gray_end);
  cauto a_max = std::max(std::max(a.r, a.g), a.b);
  cauto b_max = std::max(std::max(b.r, b.g), b.b);
  cauto ratio = (a_max + b_max) / 2.f;

  return avg_hue * 1'000 + ratio * 100'000;
}

// сортирует список палитр по цветам
inline static void sort_by_color(Strs& dst_list) {
  cauto comp = [](cr<Str> a, cr<Str> b) {
    cauto a_colors = colors_from_pal24(load_res(a));
    cauto b_colors = colors_from_pal24(load_res(b));
    cauto a_score = pal24_score(a_colors);
    cauto b_score = pal24_score(b_colors);
    return a_score > b_score;
  };
  std::sort(dst_list.begin(), dst_list.end(), comp);
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
  sort_by_color(file_list);
    
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
