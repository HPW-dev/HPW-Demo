#include <algorithm>
#include <unordered_map>
#include <exception>
#include <utility>
#include "anim-mgr.hpp"
#include "anim.hpp"
#include "frame.hpp"
#include "anim-io.hpp"
#include "util/str-util.hpp"
#include "util/error.hpp"
#include "util/log.hpp"
#include "game/core/anims.hpp"

struct Anim_mgr::Impl {
  std::unordered_map<Str, Shared<Anim>> table {};

  inline std::size_t anim_count() const { return table.size(); }

  inline void add_anim(cr<Str> str, cr<Shared<Anim>> new_anim) {
    detailed_log("Anim_mgr.add_anim: \"" << str << "\"\n");
    auto str_low = str_tolower(str);
    new_anim->set_name(str_low);
    table[str_low] = new_anim; 
  }

  inline cr<Shared<Anim>> find_anim(cr<Str> name) {
    try {
      return table.at(str_tolower(name));
    } catch (...) {
      if (hpw::lazy_load_anim) {
        detailed_log("анимация \"" << name <<
          "\" не загружена.\nзагрузка \"" << name << "\"\n");

        add_anim(name, load_from_config(name));
        try {
          return table.at(str_tolower(name));
        } catch (...) {
          error("не удалось загрузить \"" + name + "\"");
        }
      } else {
        error("Anim_mgr.find_anim: animation \"" <<
          name << "\" not found");
      }
    }
  } // find_anim

  inline cp<Direct> get_direct(cr<Str> str,
  uint frame_num, real degree) {
    auto anim {find_anim(str)};
    auto frame {anim->get_frame(frame_num)};
    auto direct {frame->get_direct(degree)};
    return direct;
  }

  inline void remove_anim(cr<Str> str)
    { table.erase(str); }

  inline Strs names() const {
    Strs v_name;
    for (crauto [key, _]: table)
      v_name.push_back(key);
    return v_name;
  }

  inline bool exist(cr<Str> str) const
    { return table.find(str_tolower(str)) != table.end(); }

  inline Anims get_anims() const {
    Anims anims;
    for (crauto [_, anim]: table)
      anims.emplace_back(anim);
    return anims;
  }

  inline void clear() { table.clear(); }

  /** ищет анимаю по имени
  * @details вернёт список анимаций и итератор на найденую */
  inline auto find_helper (cr<Impl> mgr, cr<Str> name) const {
    auto anims = mgr.get_anims();
    auto finded = std::find_if (anims.begin(), anims.end(),
      [&name](crauto in) { return in->get_name() == name; } );
    return std::tuple (
      std::move(anims),
      std::move(finded)
    );
  }

  inline std::size_t get_id(cr<Str> name) const {
    auto [anims, finded] = find_helper(*this, name);
    iferror(finded == anims.end(),
      "entity name \"" << name << "\" not found");
    return std::distance(anims.begin(), finded);
  }

  inline bool name_exist (cr<Str> name) const {
    auto [anims, finded] = find_helper(*this, name);
    return finded != anims.end();
  }

  // грузит одну анимацию из конфига
  inline Shared<Anim> load_from_config(cr<Str> name) const {
    cauto anim_yml = get_anim_config();
    cauto animations_node = anim_yml["animations"];
    return read_anim(animations_node[name]);
  }
}; // impl

Anim_mgr::Anim_mgr(): impl {new_unique<Impl>()} {}
Anim_mgr::~Anim_mgr() {}
void Anim_mgr::add_anim(cr<Str> str, cr<Shared<Anim>> new_anim) { impl->add_anim(str, new_anim); }
void Anim_mgr::remove_anim(cr<Str> str) { impl->remove_anim(str); }
cr<Shared<Anim>> Anim_mgr::find_anim(cr<Str> name) { return impl->find_anim(name); }
std::size_t Anim_mgr::get_id(cr<Str> name) const { return impl->get_id(name); }
bool Anim_mgr::name_exist(cr<Str> name) const { return impl->name_exist(name); }
cp<Direct> Anim_mgr::get_direct(cr<Str> str, uint frame_num, real degree) { return impl->get_direct(str, frame_num, degree); }
Strs Anim_mgr::names() const { return impl->names(); }
bool Anim_mgr::exist(cr<Str> str) const { return impl->exist(str); }
Anims Anim_mgr::get_anims() const { return impl->get_anims(); }
void Anim_mgr::clear() { impl->clear(); }
std::size_t Anim_mgr::anim_count() const { return impl->anim_count(); }
