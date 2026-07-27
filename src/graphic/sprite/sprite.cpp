#include <cassert>
#include "sprite.hpp"

Sprite::Sprite(cr<Sprite> other) { init(other); }
Sprite::Sprite(int new_x, int new_y) { init(new_x, new_y); }

Sprite& Sprite::operator = (cr<Sprite> other) { 
  init(other);
  return *this;
}

Sprite::Sprite(Sprite&& other)
: _image{ std::move(other._image) }
, _mask{ std::move(other._mask) } {}

Sprite& Sprite::operator = (Sprite&& other) {
  if (this == std::addressof(other))
    return *this;
  _image = std::move(other._image);
  _mask = std::move(other._mask);
  return *this;
}

void Sprite::init(cr<Sprite> other) {
  return_if (this == std::addressof(other));
  if(!other) {
    *this = {};
    return;
  }
  _image = other._image;
  _mask = other._mask;
}

void Sprite::init(int new_x, int new_y) {
  _image.init(new_x, new_y);
  _mask.init(new_x, new_y, Pal8::mask_invisible);
}

Sprite::operator bool() const {
  return_if(!_image, false);
  return_if(!_mask, false);
  return true;
}

void Sprite::set_path(cr<Str> val) noexcept {
  Resource::set_path(val);
  _image.set_path(val + "-image");
  _image.set_path(val + "-mask");
}
