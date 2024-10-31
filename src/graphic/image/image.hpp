#pragma once
#include <utility>
#include <optional>
#include "color-blend.hpp"
#include "mode-get.hpp"
#include "game/util/resource.hpp"

// 2d buff
class Image final: public Resource {
  Vector<Pal8> pix {};

  // проверяет допустимые размеры растра
  bool size_check(int x, int y) const noexcept;

public:
  const int X {};
  const int Y {};
  const int size {}; // X * Y

  ~Image() = default;
  Image() noexcept = default;
  Image(int nx, int ny, const std::optional<const Pal8> col={}) noexcept;
  Image(cr<Image> img) noexcept;
  Image(Image&& img) noexcept;
  Image& operator = (Image&& other) noexcept;
  Image& operator = (cr<Image> img) noexcept;

  inline operator bool() const noexcept { return !pix.empty(); }
  inline Pal8* data() noexcept { return pix.data(); }
  inline cp<Pal8> data() const noexcept { return pix.data(); }

  void init(int nx=0, int ny=0, std::optional<Pal8> col={}) noexcept;
  void init(cr<Image> img) noexcept;
  void init(Image&& img) noexcept;
  void free() noexcept;
  void fill(const Pal8 col) noexcept;

  inline Pal8& operator [](int i) noexcept { return pix[i]; }
  // TODO в новом стандарте заменить на []
  inline Pal8& operator ()(int x, int y) noexcept { return pix[y * X + x]; }

  inline const Pal8 operator [](int i) const noexcept { return pix[i]; }
  // TODO в новом стандарте заменить на []
  inline const Pal8 operator ()(int x, int y) const noexcept { return pix[y * X + x]; }
  inline cr<Pal8> fast_get(int x, int y) const noexcept { return pix[y * X + x]; }

  [[gnu::const]] inline auto begin() noexcept { return pix.begin(); }
  [[gnu::const]] inline auto begin() const noexcept { return pix.begin(); }
  [[gnu::const]] inline auto end() noexcept { return pix.end(); }
  [[gnu::const]] inline auto end() const noexcept { return pix.end(); }
  [[gnu::const]] inline auto cbegin() const noexcept { return pix.cbegin(); }
  [[gnu::const]] inline auto cend() const noexcept { return pix.cend(); }

  [[gnu::const]] const Pal8 get(int i, Image_get mode={}, const Pal8 default_val=Pal8::none) const noexcept;
  [[gnu::const]] const Pal8 get(int x, int y, Image_get mode={}, const Pal8 default_val=Pal8::none) const noexcept;
  [[gnu::const]] Pal8& get(int i, Image_get mode, Pal8& out_of_bound_val) noexcept;
  [[gnu::const]] Pal8& get(int x, int y, Image_get mode, Pal8& out_of_bound_val) noexcept;

  // меняет размер картинки, если уменьшается, значит буффер не перевыделяем
  void assign_resize(int x, int y) noexcept;

  // fast set without cheks
  template <blend_pf bf = &blend_past>
  void fast_set(int i, const Pal8 col, int optional) noexcept {
    auto &dst_pix {pix[i]};
    dst_pix = bf(col, dst_pix, optional);
  }

  // fast set without cheks
  template <blend_pf bf = &blend_past>
  void fast_set(int x, int y, const Pal8 col, int optional) noexcept {
    auto& dst_pix = pix[y * X + x];
    dst_pix = bf(col, dst_pix, optional);
  }
  
  // fast set with cheks
  template <blend_pf bf = &blend_past> 
  void set(int i, const Pal8 col, int optional=0) noexcept {
    if (uint(i) < uint(size))
      fast_set<bf>(i, col, optional);
  }

  // fast set with cheks
  template <blend_pf bf = &blend_past> 
  void set(int x, int y, const Pal8 col, int optional=0) noexcept {
    if (uint(x) < uint(X) && uint(y) < uint(Y))
      fast_set<bf>(x, y, col, optional);
  }

  // fast set with cheks + blend func
  void set(int i, const Pal8 col, blend_pf bf, int optional) noexcept;
  // fast set with cheks + blend func
  void set(int x, int y, const Pal8 col, blend_pf bf, int optional) noexcept;

  // изменение индексов с учётом границ картинки
  bool index_bound(int& x, int& y, Image_get mode = {}) const noexcept;
  // изменение индексов с учётом границ картинки
  bool index_bound(int& i, Image_get mode = {}) const noexcept;
}; // Image
