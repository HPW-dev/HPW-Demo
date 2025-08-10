#pragma once
#include <utility>
#include <optional>
#include "color-blend.hpp"
#include "mode-get.hpp"
#include "game/util/resource.hpp"

// 2d buff
template <class PIX_FMT>
class Image_templ final: public Resource {
  Vector<PIX_FMT> pix {};

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

  inline operator bool() const noexcept { return !pix.empty(); }
  inline PIX_FMT* data() noexcept { return pix.data(); }
  inline cp<PIX_FMT> data() const noexcept { return pix.data(); }

  void init(int nx=0, int ny=0, std::optional<PIX_FMT> col={}) noexcept;
  void init(cr<Image_templ<PIX_FMT>> img) noexcept;
  void init(Image_templ<PIX_FMT>&& img) noexcept;
  void free() noexcept;
  void fill(const PIX_FMT col) noexcept;

  inline PIX_FMT& operator [](int i) noexcept { return pix[i]; }
  // TODO в новом стандарте заменить на []
  inline PIX_FMT& operator ()(int x, int y) noexcept { return pix[y * X + x]; }

  inline const PIX_FMT operator [](int i) const noexcept { return pix[i]; }
  // TODO в новом стандарте заменить на []
  inline const PIX_FMT operator ()(int x, int y) const noexcept { return pix[y * X + x]; }
  inline cr<PIX_FMT> fast_get(int x, int y) const noexcept { return pix[y * X + x]; }

  [[gnu::const]] inline auto begin() noexcept { return pix.begin(); }
  [[gnu::const]] inline auto begin() const noexcept { return pix.begin(); }
  [[gnu::const]] inline auto end() noexcept { return pix.end(); }
  [[gnu::const]] inline auto end() const noexcept { return pix.end(); }
  [[gnu::const]] inline auto cbegin() const noexcept { return pix.cbegin(); }
  [[gnu::const]] inline auto cend() const noexcept { return pix.cend(); }

  [[gnu::const]] const PIX_FMT get(int i, Image_get mode={}, const PIX_FMT default_val=PIX_FMT::none) const noexcept;
  [[gnu::const]] const PIX_FMT get(int x, int y, Image_get mode={}, const PIX_FMT default_val=PIX_FMT::none) const noexcept;
  [[gnu::const]] PIX_FMT& get(int i, Image_get mode, PIX_FMT& out_of_bound_val) noexcept;
  [[gnu::const]] PIX_FMT& get(int x, int y, Image_get mode, PIX_FMT& out_of_bound_val) noexcept;

  // меняет размер картинки, если уменьшается, значит буффер не перевыделяем
  void assign_resize(int x, int y) noexcept;

  // fast set without cheks
  template <blend_pf bf = &blend_past>
  void fast_set(int i, const PIX_FMT col, int optional) noexcept {
    auto &dst_pix {pix[i]};
    dst_pix = bf(col, dst_pix, optional);
  }

  // fast set without cheks
  template <blend_pf bf = &blend_past>
  void fast_set(int x, int y, const PIX_FMT col, int optional) noexcept {
    auto& dst_pix = pix[y * X + x];
    dst_pix = bf(col, dst_pix, optional);
  }
  
  // fast set with cheks
  template <blend_pf bf = &blend_past> 
  void set(int i, const PIX_FMT col, int optional=0) noexcept {
    if (uint(i) < uint(size))
      fast_set<bf>(i, col, optional);
  }

  // fast set with cheks
  template <blend_pf bf = &blend_past> 
  void set(int x, int y, const PIX_FMT col, int optional=0) noexcept {
    if (uint(x) < uint(X) && uint(y) < uint(Y))
      fast_set<bf>(x, y, col, optional);
  }

  // fast set with cheks + blend func
  void set(int i, const PIX_FMT col, blend_pf bf, int optional) noexcept;
  // fast set with cheks + blend func
  void set(int x, int y, const PIX_FMT col, blend_pf bf, int optional) noexcept;

  // изменение индексов с учётом границ картинки
  bool index_bound(int& x, int& y, Image_get mode = {}) const noexcept;
  // изменение индексов с учётом границ картинки
  bool index_bound(int& i, Image_get mode = {}) const noexcept;
}; // Image_templ

using Image = Image_templ<Pal8>;
using Image_rgb24 = Image_templ<Rgb24>;
extern template class Image_templ<Pal8>;
extern template class Image_templ<Rgb24>;
