#include "scene-test-image.hpp"
#include "game/menu/item/list-item.hpp"
#include "game/util/locale.hpp"

Shared<Menu_list_item> get_test_image_list() {
  return new_shared<Menu_list_item>(
    get_locale_str("scene.graphic_menu.test_image_list.title"),
    Menu_list_item::Items {

    },
    []()->std::size_t { return 0; }
  );
}
