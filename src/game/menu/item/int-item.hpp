#pragma once
#include "item.hpp"

// пункт меню с числовыми опциями
class Menu_int_item final: public Menu_item {
  using Get_value = std::function<int ()>;
  using Set_value = std::function<void (int)>;

  utf32 name {};
  utf32 m_desc {};
  Get_value get_value_f {}; // для корректного получения текущих данных
  Set_value set_value_f {}; // для корректного изменения данных
  int speed_step {};

public:
  explicit Menu_int_item(cr<utf32> new_name, Get_value _get_value,
    Set_value _set_value, int _speed_step = 1, cr<utf32> desc={});
  void enable() override;
  utf32 to_text() const override;
  void plus() override;
  void minus() override;
  utf32 get_description() const override;
};
