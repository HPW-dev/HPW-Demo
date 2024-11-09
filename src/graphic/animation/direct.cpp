#include <utility>
#include "direct.hpp"
#include "graphic/sprite/sprite.hpp"
#include "game/util/store.hpp"
#include "game/core/sprites.hpp"

Direct::Direct(Direct&& other) { this->operator=( std::move(other) ); }

Direct* Direct::operator =(Direct&& other) {
  this->offset = std::move(other.offset);
  this->sprite = std::move(other.sprite);
  return this;
}

Direct::Direct(cr<Direct> other): Direct() { this->operator=(other); }

Direct* Direct::operator =(cr<Direct> other) {
  if (this == std::addressof(other))
    return {};

  this->offset = other.offset;

  // копию спрайта в банке создавать не надо
  if ( !other.sprite.expired()) {
    // найти в банке спайт
    auto this_sprite = hpw::sprites.find(other.sprite.lock()->get_path());
    if (this_sprite) {
      this->sprite = this_sprite;
    } else { // если нет в банке, то сгенерить копию
      crauto other_sprite = other.sprite.lock();
      rauto this_sprite = hpw::sprites.push(
        other_sprite->get_path() + ".copy",
        new_shared<Sprite>(*other_sprite)
      );
      this_sprite->set_generated(true);
      this->sprite = this_sprite;
    }
  }

  return this;
} // Direct copy c-tor
