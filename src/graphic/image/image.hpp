#pragma once
#include <utility>
#include <optional>
#include "color-blend.hpp"
#include "mode-get.hpp"
#include "game/util/resource.hpp"

// 2d buff
template <class PIX_FMT>
class Image_templ final: public Resource {
  static void unknown_type_error();

  Vector<PIX_FMT> pix {};
  using iterator = decltype(pix)::iterator;
  using const_iterator = decltype(pix)::const_iterator;

  // проверяет допустимые размеры растра
  bool size_check(int x, int y) const noexcept;

public:
  const int X {};
  const int Y {};
  const int size {}; // X * Y

  ~Image_templ() = default;
  Image_templ() noexcept = default;
  Image_templ(int nx, int ny, const std::optional<const PIX_FMT> col={}) noexcept;
  Image_templ(cr<Image_templ<PIX_FMT>> img) noexcept;
  Image_templ(Image_templ<PIX_FMT>&& img) noexcept;
  Image_templ<PIX_FMT>& operator = (Image_templ<PIX_FMT>&& other) noexcept;
  Image_templ<PIX_FMT>& operator = (cr<Image_templ<PIX_FMT>> img) noexcept;

  operator bool() const noexcept;
  PIX_FMT* data() noexcept;
  cp<PIX_FMT> data() const noexcept;

  void init(int nx=0, int ny=0, std::optional<PIX_FMT> col={}) noexcept;
  void init(cr<Image_templ<PIX_FMT>> img) noexcept;
  void init(Image_templ<PIX_FMT>&& img) noexcept;
  void free() noexcept;
  void fill(const PIX_FMT col) noexcept;

  PIX_FMT& operator [](int i) noexcept;
  // TODO в новом стандарте заменить на []
  PIX_FMT& operator ()(int x, int y) noexcept;

  const PIX_FMT operator [](int i) const noexcept;
  // TODO в новом стандарте заменить на []
  const PIX_FMT operator ()(int x, int y) const noexcept;
  cr<PIX_FMT> fast_get(int x, int y) const noexcept;

  [[gnu::const]] iterator begin() noexcept;
  [[gnu::const]] const_iterator begin() const noexcept;
  [[gnu::const]] iterator end() noexcept;
  [[gnu::const]] const_iterator end() const noexcept;
  [[gnu::const]] const_iterator cbegin() const noexcept;
  [[gnu::const]] const_iterator cend() const noexcept;

  [[gnu::const]] const PIX_FMT get(int i, Image_get mode={}, const PIX_FMT default_val=PIX_FMT::neutral()) const noexcept;
  [[gnu::const]] const PIX_FMT get(int x, int y, Image_get mode={}, const PIX_FMT default_val=PIX_FMT::neutral()) const noexcept;
  [[gnu::const]] PIX_FMT& get(int i, Image_get mode, PIX_FMT& out_of_bound_val) noexcept;
  [[gnu::const]] PIX_FMT& get(int x, int y, Image_get mode, PIX_FMT& out_of_bound_val) noexcept;

  // меняет размер картинки, если уменьшается, значит буффер не перевыделяем
  void assign_resize(int x, int y) noexcept;

  // fast set with cheks + blend func
  void set(int i, const PIX_FMT col, blend_pf bf, int optional) noexcept;
  // fast set with cheks + blend func
  void set(int x, int y, const PIX_FMT col, blend_pf bf, int optional) noexcept;

  // изменение индексов с учётом границ картинки
  bool index_bound(int& x, int& y, Image_get mode = {}) const noexcept;
  // изменение индексов с учётом границ картинки
  bool index_bound(int& i, Image_get mode = {}) const noexcept;

  template <blend_pf bf = &blend_past>
  inline void fast_set(int i, const PIX_FMT col, int optional) noexcept {
    auto &dst_pix {pix[i]};
    if constexpr (std::is_same_v<PIX_FMT, Pal8>) {
      dst_pix = bf(col, dst_pix, optional);
    } elif constexpr (std::is_same_v<PIX_FMT, Rgb24>) {
      // TODO blend funcs for Rgb24 is not supported
      dst_pix = col;
    } else {
      unknown_type_error();
    }
  }

  template <blend_pf bf = &blend_past>
  inline void fast_set(int x, int y, const PIX_FMT col, int optional) noexcept {
    auto& dst_pix = pix[y * X + x];
    if constexpr (std::is_same_v<PIX_FMT, Pal8>) {
      dst_pix = bf(col, dst_pix, optional);
    } elif constexpr (std::is_same_v<PIX_FMT, Rgb24>) {
      // TODO blend funcs for Rgb24 is not supported
      dst_pix = col;
    } else {
      unknown_type_error();
    }
  }
    
  template <blend_pf bf = &blend_past> 
  inline void set(int i, const PIX_FMT col, int optional=0) noexcept {
    if (uint(i) < uint(size))
      fast_set<bf>(i, col, optional);
  }

  template <blend_pf bf = &blend_past> 
  inline void set(int x, int y, const PIX_FMT col, int optional=0) noexcept {
    if (uint(x) < uint(X) && uint(y) < uint(Y))
      fast_set<bf>(x, y, col, optional);
  }
}; // Image_templ

using Image = Image_templ<Pal8>;
using Image_rgb24 = Image_templ<Rgb24>;
extern template class Image_templ<Pal8>;
extern template class Image_templ<Rgb24>;
