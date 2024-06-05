#include <cassert>
#include "sprite.hpp"

Sprite::Sprite(CN<Sprite> other) noexcept { init(other); }
Sprite::Sprite(int new_x, int new_y) noexcept { init(new_x, new_y); }

Sprite& Sprite::operator = (CN<Sprite> other) noexcept { 
  init(other);
  return *this;
}

Sprite::Sprite(Sprite&& other) noexcept
: m_image{ std::move(other.m_image) }
, m_mask{ std::move(other.m_mask) } {}

Sprite& Sprite::operator = (Sprite&& other) noexcept {
  if (this == std::addressof(other))
    return *this;
  m_image = std::move(other.m_image);
  m_mask = std::move(other.m_mask);
  return *this;
}

void Sprite::init(CN<Sprite> other) noexcept {
  return_if (this == std::addressof(other));
  if(!other) {
    *this = {};
    return;
  }
  m_image = other.m_image;
  m_mask = other.m_mask;
}

void Sprite::init(int new_x, int new_y) noexcept {
  m_image.init(new_x, new_y);
  m_mask.init(new_x, new_y, Pal8::mask_invisible);
}

Sprite::operator bool() const {
  return_if(!m_image, false);
  return_if(!m_mask, false);
  return true;
}
