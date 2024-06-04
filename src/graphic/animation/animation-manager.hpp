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
  void add_anim(CN<Str> str, CN<Shared<Anim>> new_anim);
  void remove_anim(CN<Str> str);
  // получить анимацию по её имени
  CN<Shared<Anim>> find_anim(CN<Str> name) const;
  // узнать id анимации по её имени
  std::size_t get_id(CN<Str> name) const;
  // узнать что есть объект с таким именем (true если есть)
  bool name_exist (CN<Str> name) const;
  // получить развороты конкретного фрейма
  CP<Direct> get_direct(CN<Str> str, uint frame_num, real degree);
  Strs names() const;
  // проверить наличие анимации по имени
  bool exist(CN<Str> str) const;
  Anims get_anims() const;
  void clear();
  std::size_t anim_count() const;
}; // Anim_mgr
