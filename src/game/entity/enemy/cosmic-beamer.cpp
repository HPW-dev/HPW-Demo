#include <cassert>
#include "cosmic-beamer.hpp"
#include "game/entity/util/phys.hpp"
#include "game/entity/entity-manager.hpp"
#include "game/game-common.hpp"
#include "graphic/image/image.hpp"
#include "util/file/yaml.hpp"

Cosmic_beamer::Cosmic_beamer(): Proto_enemy(GET_SELF_TYPE) {}

void Cosmic_beamer::draw(Image& dst, const Vec offset) const {
  Proto_enemy::draw(dst, offset);
  // TODO ?
}

void Cosmic_beamer::update(double dt) {
  assert(hpw::shmup_mode); // вне шмап-мода этот класс не юзать
  Proto_enemy::update(dt);
  // TODO
}

struct Cosmic_beamer::Loader::Impl {
  inline explicit Impl(CN<Yaml> config) {
    // TODO
  } // c-tor

  inline Entity* operator()(Entity* master, const Vec pos, Entity* parent) {
    assert(parent);
    // TODO
    return parent;
  } // op ()

}; // Impl

Cosmic_beamer::Loader::Loader(CN<Yaml> config)
: Proto_enemy::Loader(config)
, impl{new_unique<Impl>(config)}
{}

Entity* Cosmic_beamer::Loader::operator()(Entity* master, const Vec pos, Entity* parent) {
  auto ret = hpw::entity_mgr->allocate<Cosmic_beamer>();
  Proto_enemy::Loader::operator()(master, pos, ret);
  impl->operator()(master, pos, ret);
  return ret;
}

Cosmic_beamer::Loader::~Loader() {}
