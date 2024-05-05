#pragma once
#include "item.hpp"

// на этот пункт меню можно только нажимать
class Menu_text_item final: public Menu_item {
  menu_item_ft action = {};
  utf32 name = {};
  utf32 m_desc {};
  std::function<utf32 ()> get_value = {};

public:
  explicit Menu_text_item(CN<utf32> _name, menu_item_ft&& _action,
    decltype(get_value)&& _get_value = {}, CN<utf32> desc={});
  void enable() override;
  utf32 to_text() const override;
  utf32 get_description() const override;
};
