#include <utility>
#include "direct.hpp"
#include "graphic/sprite/sprite.hpp"
#include "game/game-common.hpp"
#include "game/util/store.hpp"

Direct::Direct(Direct&& other) { this->operator=( std::move(other) ); }

Direct* Direct::operator =(Direct&& other) {
  this->offset = std::move(other.offset);
  this->sprite = std::move(other.sprite);
  return this;
}

Direct::Direct(CN<Direct> other): Direct() { this->operator=(other); }

Direct* Direct::operator =(CN<Direct> other) {
  if (this == std::addressof(other))
    return {};

  this->offset = other.offset;

  // копию спрайта в банке создавать не надо
  if ( !other.sprite.expired()) {
    // найти в банке спайт
    auto this_sprite = hpw::store_sprite->find(other.sprite.lock()->get_path());
    if (this_sprite) {
      this->sprite = this_sprite;
    } else { // если нет в банке, то сгенерить копию
      cnauto other_sprite = other.sprite.lock();
      nauto this_sprite = hpw::store_sprite->push(
        other_sprite->get_path() + ".copy",
        new_shared<Sprite>(*other_sprite)
      );
      this_sprite->set_generated(true);
      this->sprite = this_sprite;
    }
  }

  return this;
} // Direct copy c-tor
