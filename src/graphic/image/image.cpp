#include <cassert>
#include <cstring>
#include <algorithm>
#include "image.hpp"
#include "util/macro.hpp"
#include "util/log.hpp"

Image::Image(CN<Image> img) noexcept { init(img); }
Image::Image(Image&& other) noexcept { *this = std::move(other); }

Image& Image::operator = (Image&& other) noexcept {
  if (this != std::addressof(other)) {
    other.swap(*this);
    other.free();
  }
  return *this;
}

Image& Image::operator = (CN<Image> other) noexcept {
  if (this != std::addressof(other))
    Image(other).swap(*this);
  return *this;
}

void Image::swap(Image& other) noexcept {
  std::swap(other.pix, pix);
  std::swap(ccast<int&>(other.X), ccast<int&>(X));
  std::swap(ccast<int&>(other.Y), ccast<int&>(Y));
  std::swap(ccast<int&>(other.size), ccast<int&>(size));
}

Image::Image(int nx, int ny, std::optional<Pal8> col) noexcept
: X {nx}
, Y {ny}
, size {nx * ny}
{
  if ( !size_check(nx, ny)) {
    free();
    return;
  } else {
    pix = decltype(pix)(size);
  }
  if (col)
    fill(*col);
} // c-tor

// выделение памяти под картинку
void Image::init(int nx, int ny, std::optional<Pal8> col) noexcept {
  if ( !size_check(nx, ny)) {
    free();
    return;
  }
  ccast<int&>(X) = nx;
  ccast<int&>(Y) = ny;
  ccast<int&>(size) = nx * ny;
  // можно не перевыделять память, если картинка была большой
  if (std::size_t(size) > pix.size())
    pix.resize(size);
  if (col)
    fill(*col);
} // init

void Image::fill(const Pal8 col) noexcept
  { memset(scast<void*>(pix.data()), col.val, pix.size()); }

// копирование
void Image::init(CN<Image> img) noexcept {
  return_if (this == std::addressof(img));
  init(img.X, img.Y);
  pix = img.pix;
}

const Pal8 Image::get(int i, Image_get mode) const {
  if (*this && index_bound(i, mode))
    return operator[](i);
  static Pal8 none_color {Pal8::none};
  return none_color;
}

const Pal8 Image::get(int x, int y, Image_get mode) const {
  if (*this && index_bound(x, y, mode))
    return operator()(x, y);
  static Pal8 none_color {Pal8::none};
  return none_color;
}

bool Image::index_bound(int& x, int& y, Image_get mode) const {
  switch (mode) {
    default:
    case Image_get::NONE: // fast ver of x >= 0 && x < X && y >= 0...:
      return uint(x) < uint(X) && uint(y) < uint(Y);
      break;
    case Image_get::MIRROR:
      if (x < 0)
        x = x % X * -1;
      if (x >= X)
        x = X - (x % X) - 1;
      if (y < 0)
        y = y % Y * -1;
      if (y >= Y)
        y = Y - (y % Y) - 1;
      break;
    case Image_get::DIV:
      if (x < 0)
        x *= -1;
      if (x >= X)
        x %= X;
      if (y < 0)
        y *= -1;
      if (y >= Y)
        y %= Y;
      break;
    case Image_get::COPY:
      if (x < 0)
        x = 0;
      if (x >= X)
        x = X-1;
      if (y < 0)  
        y = 0;
      if (y >= Y)
        y = Y-1;
      break;
  }
  return true;
} // index_bound x y

bool Image::index_bound(int& i, Image_get mode) const {
  if (X < 1 || Y < 1)
    return false;
  switch (mode) {
    case Image_get::MIRROR: {
      if (i < 0)
        i = i % size * -1;
      if (i >= size)
        i = size - (i % size) - 1;
      break;
    }
    case Image_get::DIV: {
      if (i < 0)
        i *= -1;
      if (i >= size)
        i %= size;
      break;
    }
    case Image_get::COPY: {
      if (i < 0)
        i = 0;
      if (i >= size)
        i = size-1;
      break;
    }
    case Image_get::NONE:
    default: { return i >= 0 && i < size; }
  }
  return true;
} // index_bound i

void Image::assign_resize(int x, int y) noexcept {
  auto new_size = std::size_t(x * y);
  return_if (new_size == 0);
  if (new_size <= pix.size()) {
    ccast<int&>(X) = x;
    ccast<int&>(Y) = y;
    ccast<int&>(size) = new_size;
    pix.resize(new_size);
  } else {
    init(x, y);
  }
}

bool Image::size_check(int x, int y) const noexcept {
  bool ret = x > 0 && x <= 4'048 && y > 0 && y <= 4'048;
  detailed_iflog( !ret, "warning: bad sizes for image init");
  return ret;
}

void Image::free() noexcept {
  pix.clear();
  ccast<int&>(X) = 0;
  ccast<int&>(Y) = 0;
  ccast<int&>(size) = 0;
}

void Image::set(int i, const Pal8 col, blend_pf bf, int optional) {
  auto src = this->get(i);
  this->set(i, bf(col, src, optional));
}

void Image::set(int x, int y, const Pal8 col, blend_pf bf, int optional) {
  auto src = this->get(x, y);
  this->set(x, y, bf(col, src, optional));
}
