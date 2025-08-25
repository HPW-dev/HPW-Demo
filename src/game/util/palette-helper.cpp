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

// среднее здачение цвета из палитры
inline static HSL hsl_weighted_average(cr<Vector<Rgb24>> pal24) {
  assert(pal24.size() >= 256);

  // брать только первую часть палитры без красных оттенков
  Vector<Rgb24> main_colors(pal24.begin(), pal24.begin() + Pal8::gray_size);
  HSL avg;
  for (uint i = 0; cauto rgb24: main_colors) {
    auto hsl = rgb24_to_hsl(rgb24);
    // для разных частей палитры разные веса
    real w;
    if      (i <   Pal8::gray_size/3)    w = 0.1;
    else if (i <  (Pal8::gray_size/3*2)) w = 0.6;
    else if (i >= (Pal8::gray_size/3*2)) w = 0.3;
    avg.h += hsl.h * w;
    avg.s += hsl.s * w;
    avg.l += hsl.l * w;
    ++i;
  }
  avg.h /= main_colors.size();
  avg.s /= main_colors.size();
  avg.l /= main_colors.size();
  return avg;
}

// сортирует список палитр по цветам
inline static void sort_by_color(Strs& dst_list) {
  cauto comp = [](cr<Str> a, cr<Str> b) {
    cauto a_colors = colors_from_pal24(load_res(a));
    cauto b_colors = colors_from_pal24(load_res(b));
    cauto a_avg = hsl_weighted_average(a_colors);
    cauto b_avg = hsl_weighted_average(b_colors);
    // Приоритет: оттенок -> насыщенность -> яркость
    if (std::abs(a_avg.h - b_avg.h) > 10.0) return a_avg.h < b_avg.h;
    if (std::abs(a_avg.s - b_avg.s) > 0.1)  return a_avg.s < b_avg.s;
    return a_avg.l < b_avg.l;
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
