#include <cassert>
#include <utility>
#include "sprite.hpp"
#include "graphic/image/image.hpp"

void Sprite::set_image(CN<Image> image) noexcept
  { _image = new_shared<Image>(image); }

void Sprite::set_mask(CN<Image> mask) noexcept
  { _mask = new_shared<Image>(mask); }

void Sprite::move_image(Image&& image) noexcept
  { _image = new_shared<Image>(std::move(image)); }

void Sprite::move_mask(Image&& mask) noexcept
  { _mask = new_shared<Image>(std::move(mask)); }

Sprite::Sprite(CN<Sprite> other) noexcept { init(other); }

Sprite::Sprite(int new_x, int new_y) noexcept { init(new_x, new_y); }

Sprite& Sprite::operator = (CN<Sprite> other) noexcept { 
  init(other);
  return *this;
}

Sprite::Sprite(Sprite&& other) noexcept
: _image(std::move(other._image))
, _mask(std::move(other._mask)) {}

Sprite& Sprite::operator = (Sprite&& other) noexcept {
  if (this == std::addressof(other))
    return *this;
  _image = std::move(other._image);
  _mask = std::move(other._mask);
  return *this;
}

void Sprite::init(CN<Sprite> other) noexcept {
  return_if (this == std::addressof(other));
  set_image(*other.get_image());
  set_mask(*other.get_mask());
}

void Sprite::init(int new_x, int new_y) noexcept {
  move_image(std::move( Image(new_x, new_y) ));
  move_mask(std::move( Image(new_x, new_y, Pal8::mask_invisible) ));
}

Sprite::operator bool() const {
  if ( !_image)
    return false;
  if ( !*_image)
    return false;
  if ( !_mask)
    return false;
  if ( !*_mask)
    return false;
  return true;
}

int Sprite::X() const { return _image ? _image->X : 0; }
int Sprite::Y() const { return _image ? _image->Y : 0; }
