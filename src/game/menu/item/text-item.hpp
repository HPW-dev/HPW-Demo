#pragma once
#include "item.hpp"

// на этот пункт меню можно только нажимать
class Menu_text_item final: public Menu_item {
  menu_item_ft action = {};
  utf32 name = {};
  utf32 m_desc {};
  std::function<utf32 ()> get_value = {};

public:
  explicit Menu_text_item(cr<utf32> _name, menu_item_ft&& _action,
    decltype(get_value)&& _get_value = {}, cr<utf32> desc={});
  void enable() override;
  utf32 to_text() const override;
  utf32 get_description() const override;
  inline void plus() override { enable(); }
  inline void plus_fast() override { enable(); }
  inline void minus() override { enable(); }
  inline void minus_fast() override { enable(); }
};
