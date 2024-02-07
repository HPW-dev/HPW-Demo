#pragma once
#include <optional>
#include "color-blend.hpp"
#include "mode-get.hpp"
#include "game/util/resource.hpp"

/// 2d buff
class Image final: public Resource {
  Vector<Pal8> pix {};

  /// проверяет допустимые размеры растра
  bool size_check(int x, int y) const noexcept;

public:
  const int X {};
  const int Y {};
  const int size {}; /// X * Y

  ~Image() = default;
  Image() noexcept = default;
  Image(int nx, int ny, std::optional<Pal8> col={}) noexcept;
  Image(CN<Image> img) noexcept;
  Image(Image&& img) noexcept;
  Image& operator = (Image&& other) noexcept;
  Image& operator = (CN<Image> img) noexcept;

  inline operator bool() const noexcept { return !pix.empty(); }
  inline Pal8* data() { return pix.data(); }
  inline CP<Pal8> data() const { return pix.data(); }

  void init(int nx=0, int ny=0, std::optional<Pal8> col={}) noexcept;
  void init(CN<Image> img) noexcept;
  void free() noexcept;
  void swap(Image& other) noexcept;
  void fill(const Pal8 col) noexcept;

  inline Pal8& operator [](int i) { return pix[i]; }
  /// TODO в новом стандарте заменить на []
  inline Pal8& operator ()(int x, int y) { return pix[y * X + x]; }

  inline const Pal8 operator [](int i) const { return pix[i]; }
  /// TODO в новом стандарте заменить на []
  inline const Pal8 operator ()(int x, int y) const { return pix[y * X + x]; }

  inline auto begin() { return pix.begin(); }
  inline auto begin() const { return pix.begin(); }
  inline auto end() { return pix.end(); }
  inline auto end() const { return pix.end(); }

  const Pal8 get(int i, Image_get mode = {}) const;
  const Pal8 get(int x, int y, Image_get mode = {}) const;

  /// меняет размер картинки, если уменьшается, значит буффер не перевыделяем
  void assign_resize(int x, int y) noexcept;

  /// fast set without cheks
  template <blend_pf bf = &blend_past>
  void fast_set(int i, const Pal8 col, int optional) {
    auto &dst_pix {pix[i]};
    dst_pix = bf(col, dst_pix, optional);
  }

  /// fast set without cheks
  template <blend_pf bf = &blend_past>
  void fast_set(int x, int y, const Pal8 col, int optional) {
    auto &dst_pix {pix[y * X + x]};
    dst_pix = bf(col, dst_pix, optional);
  }
  
  /// fast set with cheks
  template <blend_pf bf = &blend_past> 
  void set(int i, const Pal8 col, int optional=0) {
    if (uint(i) < uint(size))
      fast_set<bf>(i, col, optional);
  }

  /// fast set with cheks
  template <blend_pf bf = &blend_past> 
  void set(int x, int y, const Pal8 col, int optional=0) {
    if (uint(x) < uint(X) && uint(y) < uint(Y))
      fast_set<bf>(x, y, col, optional);
  }

  /// fast set with cheks + blend func
  void set(int i, const Pal8 col, blend_pf bf, int optional);
  /// fast set with cheks + blend func
  void set(int x, int y, const Pal8 col, blend_pf bf, int optional);

  /// изменение индексов с учётом границ картинки
  bool index_bound(int& x, int& y, Image_get mode = {}) const;
  /// изменение индексов с учётом границ картинки
  bool index_bound(int& i, Image_get mode = {}) const;
}; // Image
