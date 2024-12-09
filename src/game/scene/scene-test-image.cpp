#include <cassert>
#include <algorithm>
#include "scene-test-image.hpp"
#include "game/menu/item/list-item.hpp"
#include "game/core/graphic.hpp"
#include "game/core/common.hpp"
#include "game/core/sprites.hpp"
#include "game/util/locale.hpp"
#include "game/util/resource-helper.hpp"
#include "graphic/util/graphic-util.hpp"
#include "util/path.hpp"

struct Test_image_path {
  Str full_path {};
  Str short_name {};
};
using Test_image_paths = Vector<Test_image_path>;

// список картинок в папке с тестовыми картинками
static inline Test_image_paths get_test_image_paths() {
  auto res_names = get_all_res_names();

  // вырезать всё, что не в той папке
  std::erase_if(res_names, [](cr<Str> path) {
    const bool is_test_image_dir = path.find(graphic::TEST_IMAGES_DIR) == str_npos;
    return is_test_image_dir || path == graphic::TEST_IMAGES_DIR;
  });

  std::sort(res_names.begin(), res_names.end());
  assert(!res_names.empty());

  Test_image_paths ret;
  for (crauto res_name: res_names) {
    ret.push_back( Test_image_path {
      .full_path = res_name,
      .short_name = get_filename(res_name)
    });
  }
  return ret;
}

// список картинок на выбор
static inline Menu_list_item::Items get_items() {
  cauto test_image_paths = get_test_image_paths();

  Menu_list_item::Items ret;
  for (crauto test_image_path: test_image_paths) {
    ret.push_back( Menu_list_item::Item {
      .name = utf8_to_32(test_image_path.short_name),
      .desc = {},
      .action = [name = test_image_path.full_path] { graphic::cur_test_image_path = name; }
    } );
  }
  return ret;
}

// указать какое изначально выбрано тестовое изображение
static inline std::size_t get_default_item_id() {
  cauto image_names = get_test_image_paths();
  
  for (std::size_t ret = 0; crauto image_name: image_names) {
    if (image_name.full_path == graphic::cur_test_image_path)
      return ret;
    ++ret;
  }

  return 0;
}

Shared<Menu_list_item> get_test_image_list() {
  return new_shared<Menu_list_item>(
    get_locale_str("scene.graphic_menu.test_image_list.title"),
    get_items(), &get_default_item_id
  );
}

void draw_test_image(Image& dst) {
  cauto _test_image = hpw::sprites.find(graphic::cur_test_image_path);
  return_if(!_test_image);
  insert_fast(dst, _test_image->image());
}
