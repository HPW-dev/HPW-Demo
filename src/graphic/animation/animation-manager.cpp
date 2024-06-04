#include <algorithm>
#include <unordered_map>
#include <exception>
#include <utility>
#include "animation-manager.hpp"
#include "anim.hpp"
#include "frame.hpp"
#include "util/str-util.hpp"
#include "util/error.hpp"
#include "util/log.hpp"

struct Anim_mgr::Impl {
  std::unordered_map<Str, Shared<Anim>> table {};

  inline std::size_t anim_count() const { return table.size(); }

  inline void add_anim(CN<Str> str, CN<Shared<Anim>> new_anim) {
    detailed_log("Anim_mgr.add_anim: \"" << str << "\"\n");
    auto str_low = str_tolower(str);
    new_anim->set_name(str_low);
    table[str_low] = new_anim; 
  }

  inline CN<Shared<Anim>> find_anim(CN<Str> name) const {
    try {
      return table.at(str_tolower(name));
    } catch (...) {
      auto msg = "Anim_mgr.find_anim: animation \"" +
        name + "\" not found\n";
      error(msg);
    }
  }

  inline CP<Direct> get_direct(CN<Str> str,
  uint frame_num, real degree) {
    auto anim {find_anim(str)};
    auto frame {anim->get_frame(frame_num)};
    auto direct {frame->get_direct(degree)};
    return direct;
  }

  inline void remove_anim(CN<Str> str)
    { table.erase(str); }

  inline Strs names() const {
    Strs v_name;
    for (cnauto [key, _]: table)
      v_name.push_back(key);
    return v_name;
  }

  inline bool exist(CN<Str> str) const {
    try {
      table.at(str_tolower(str));
    } catch (...) {
      return false;
    }
    return true;
  }

  inline Anims get_anims() const {
    Anims anims;
    for (cnauto [_, anim]: table)
      anims.emplace_back(anim);
    return anims;
  }

  inline void clear() { table.clear(); }

  /** ищет анимаю по имени
  * @details вернёт список анимаций и итератор на найденую */
  inline auto find_helper (CN<Impl> mgr, CN<Str> name) const {
    auto anims = mgr.get_anims();
    auto finded = std::find_if (anims.begin(), anims.end(),
      [&name](cnauto in) { return in->get_name() == name; } );
    return std::tuple (
      std::move(anims),
      std::move(finded)
    );
  }

  inline std::size_t get_id(CN<Str> name) const {
    auto [anims, finded] = find_helper(*this, name);
    iferror(finded == anims.end(),
      "entity name \"" << name << "\" not found");
    return std::distance(anims.begin(), finded);
  }

  inline bool name_exist (CN<Str> name) const {
    auto [anims, finded] = find_helper(*this, name);
    return finded != anims.end();
  }
}; // impl

Anim_mgr::Anim_mgr(): impl {new_unique<Impl>()} {}
Anim_mgr::~Anim_mgr() {}
void Anim_mgr::add_anim(CN<Str> str, CN<Shared<Anim>> new_anim) { impl->add_anim(str, new_anim); }
void Anim_mgr::remove_anim(CN<Str> str) { impl->remove_anim(str); }
CN<Shared<Anim>> Anim_mgr::find_anim(CN<Str> name) const { return impl->find_anim(name); }
std::size_t Anim_mgr::get_id(CN<Str> name) const { return impl->get_id(name); }
bool Anim_mgr::name_exist(CN<Str> name) const { return impl->name_exist(name); }
CP<Direct> Anim_mgr::get_direct(CN<Str> str, uint frame_num, real degree) { return impl->get_direct(str, frame_num, degree); }
Strs Anim_mgr::names() const { return impl->names(); }
bool Anim_mgr::exist(CN<Str> str) const { return impl->exist(str); }
Anims Anim_mgr::get_anims() const { return impl->get_anims(); }
void Anim_mgr::clear() { impl->clear(); }
std::size_t Anim_mgr::anim_count() const { return impl->anim_count(); }
