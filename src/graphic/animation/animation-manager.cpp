#include <algorithm>
#include <exception>
#include <utility>
#include "animation-manager.hpp"
#include "anim.hpp"
#include "frame.hpp"
#include "util/str-util.hpp"
#include "util/error.hpp"
#include "util/log.hpp"

void Anim_mgr::add_anim(CN<Str> str, CN<Shared<Anim>> new_anim) {
  detailed_log("Anim_mgr.add_anim: \"" << str << "\"\n");
  auto str_low = str_tolower(str);
  new_anim->set_name(str_low);
  table[str_low] = new_anim; 
}

CN<Shared<Anim>> Anim_mgr::find_anim(CN<Str> name) const {
  try {
    return table.at(str_tolower(name));
  } catch (...) {
    auto msg = "Anim_mgr.find_anim: animation \"" +
      name + "\" not found\n";
    error(msg);
  }
}

CP<Direct> Anim_mgr::get_direct(CN<Str> str, uint frame_num,
real degree) {
  auto anim {find_anim(str)};
  auto frame {anim->get_frame(frame_num)};
  auto direct {frame->get_direct(degree)};
  return direct;
}

void Anim_mgr::remove_anim(CN<Str> str)
  { table.erase(str); }

Strs Anim_mgr::names() const {
  Strs v_name;
  for (cnauto [key, _]: table)
    v_name.push_back(key);
  return v_name;
}

bool Anim_mgr::exist(CN<Str> str) const {
  try {
    table.at(str_tolower(str));
  } catch (...) {
    return false;
  }
  return true;
}

Anims Anim_mgr::get_anims() const {
  Anims anims;
  for (cnauto [_, anim]: table)
    anims.emplace_back(anim);
  return anims;
}

void Anim_mgr::clear() { table.clear(); }

/** ищет анимаю по имени
* @details вернёт список анимаций и итератор на найденую */
inline auto find_helper (CN<Anim_mgr> mgr, CN<Str> name) {
  auto anims = mgr.get_anims();
  auto finded = std::find_if (anims.begin(), anims.end(),
    [&name](cnauto in) { return in->get_name() == name; } );
  return std::tuple (
    std::move(anims),
    std::move(finded)
  );
} // find_helper

std::size_t Anim_mgr::get_id(CN<Str> name) const {
  auto [anims, finded] = find_helper(*this, name);
  iferror(finded == anims.end(),
    "entity name \"" << name << "\" not found");
  return std::distance(anims.begin(), finded);
}

bool Anim_mgr::name_exist (CN<Str> name) const {
  auto [anims, finded] = find_helper(*this, name);
  return finded != anims.end();
}
