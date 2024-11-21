#include "menu-util.hpp"
#include "item/text-item.hpp"

Shared<Menu_item> make_menu_separator(bool* flag) {
  constexpr auto ITEM_SEPARATOR = U"- - - - - - - - - - - - - - - - - -";

  return new_shared<Menu_text_item>(ITEM_SEPARATOR, [flag]{
    if (flag)
      *flag = true;
  });
}
