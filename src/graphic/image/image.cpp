#include <cassert>
#include <cstring>
#include <utility>
#include <algorithm>
#include "image.hpp"
#include "util/macro.hpp"
#include "util/log.hpp"

#define IMGTMPL Image_templ<PIX_FMT>
template <class PIX_FMT> IMGTMPL::Image_templ(cr<IMGTMPL> img) noexcept { init(img); }
template <class PIX_FMT> IMGTMPL::Image_templ(IMGTMPL&& other) noexcept { *this = std::move(other); }

template <class PIX_FMT>
IMGTMPL& IMGTMPL::operator = (IMGTMPL&& other) noexcept {
  return_if (this == std::addressof(other), *this);
  pix = std::move(other.pix);
  ccast<int&>(X) = other.X;
  ccast<int&>(Y) = other.Y;
  ccast<int&>(size) = other.size;
  other.free();
  return *this;
}

template <class PIX_FMT>
IMGTMPL& IMGTMPL::operator = (cr<IMGTMPL> other) noexcept {
  return_if (this == std::addressof(other), *this);
  pix = other.pix;
  ccast<int&>(X) = other.X;
  ccast<int&>(Y) = other.Y;
  ccast<int&>(size) = other.size;
  return *this;
}

template <class PIX_FMT>
IMGTMPL::Image_templ(int nx, int ny, const std::optional<const PIX_FMT> col) noexcept
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

template <class PIX_FMT>
void IMGTMPL::init(int nx, int ny, std::optional<PIX_FMT> col) noexcept {
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

template <class PIX_FMT>
void IMGTMPL::fill(const PIX_FMT col) noexcept
  { memset(scast<void*>(pix.data()), col.val, pix.size()); }

template <class PIX_FMT>
void IMGTMPL::init(cr<IMGTMPL> img) noexcept {
  return_if (this == std::addressof(img));
  init(img.X, img.Y);
  pix = img.pix;
}

template <class PIX_FMT>
void IMGTMPL::init(IMGTMPL&& img) noexcept {
  return_if (this == std::addressof(img));
  pix = std::move(img.pix);
  ccast<int&>(X) = img.X;
  ccast<int&>(Y) = img.Y;
  ccast<int&>(size) = img.size;
  img.free();
}

template <class PIX_FMT>
[[gnu::const]] const PIX_FMT IMGTMPL::get(int i, Image_get mode, const PIX_FMT default_val) const noexcept {
  if (*this && index_bound(i, mode))
    return operator[](i);
  return default_val;
}

template <class PIX_FMT>
[[gnu::const]] const PIX_FMT IMGTMPL::get(int x, int y, Image_get mode, const PIX_FMT default_val) const noexcept {
  if (*this && index_bound(x, y, mode))
    return operator()(x, y);
  return default_val;
}

template <class PIX_FMT>
[[gnu::const]] PIX_FMT& IMGTMPL::get(int i, Image_get mode, PIX_FMT& out_of_bound_val) noexcept {
  if (*this && index_bound(i, mode))
    return operator[](i);
  return out_of_bound_val;
}

template <class PIX_FMT>
[[gnu::const]] PIX_FMT& IMGTMPL::get(int x, int y, Image_get mode, PIX_FMT& out_of_bound_val) noexcept {
  if (*this && index_bound(x, y, mode))
    return operator()(x, y);
  return out_of_bound_val;
}

template <class PIX_FMT>
bool IMGTMPL::index_bound(int& x, int& y, Image_get mode) const noexcept {
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

template <class PIX_FMT>
bool IMGTMPL::index_bound(int& i, Image_get mode) const noexcept {
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

template <class PIX_FMT>
void IMGTMPL::assign_resize(int x, int y) noexcept {
  const std::size_t new_size = x * y;
  assert(new_size > 0);
  assert(new_size <= 16'000 * 16'000);

  if (new_size <= pix.size()) {
    ccast<int&>(X) = x;
    ccast<int&>(Y) = y;
    ccast<int&>(size) = new_size;
    pix.resize(new_size);
  } else {
    init(x, y);
  }
}

template <class PIX_FMT>
bool IMGTMPL::size_check(int x, int y) const noexcept {
  const bool ret = x > 0 && x <= 1024 * 16 && y > 0 && y <= 1024 * 16;

  if (!ret)
    log_warning << "bad sizes for image init";

  return ret;
}

template <class PIX_FMT>
void IMGTMPL::free() noexcept {
  pix.clear();
  ccast<int&>(X) = 0;
  ccast<int&>(Y) = 0;
  ccast<int&>(size) = 0;
}

template <class PIX_FMT>
void IMGTMPL::set(int i, const PIX_FMT col, blend_pf bf, int optional) noexcept {
  auto src = this->get(i);
  this->set(i, bf(col, src, optional));
}

template <class PIX_FMT>
void IMGTMPL::set(int x, int y, const PIX_FMT col, blend_pf bf, int optional) noexcept {
  auto src = this->get(x, y);
  this->set(x, y, bf(col, src, optional));
}

extern template class Image_templ<Pal8>;
extern template class Image_templ<Rgb24>;
