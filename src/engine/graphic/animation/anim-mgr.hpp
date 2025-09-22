#pragma once
#include "util/str.hpp"
#include "util/mem-types.hpp"
#include "util/math/num-types.hpp"
#include "util/macro.hpp"

class Anim;
struct Direct;
using Anims = Vector<Shared<Anim>>;

// доступ к анимации
class Anim_mgr final {
  struct Impl;
  Unique<Impl> impl {};

public:
  Anim_mgr();
  ~Anim_mgr();
  void add_anim(cr<Str> str, cr<Shared<Anim>> new_anim);
  void remove_anim(cr<Str> str);
  // получить анимацию по её имени
  cr<Shared<Anim>> find_anim(cr<Str> name);
  // узнать id анимации по её имени
  std::size_t get_id(cr<Str> name) const;
  // узнать что есть объект с таким именем (true если есть)
  bool name_exist (cr<Str> name) const;
  // получить развороты конкретного фрейма
  cp<Direct> get_direct(cr<Str> str, uint frame_num, real degree);
  Strs names() const;
  // проверить наличие анимации по имени
  bool exist(cr<Str> str) const;
  Anims get_anims() const;
  void clear();
  std::size_t anim_count() const;
}; // Anim_mgr
