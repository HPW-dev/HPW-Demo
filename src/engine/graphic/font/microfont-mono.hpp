#include "font.hpp"
#include "util/mem-types.hpp"

// Пиксельный ASCI шрифт
class Microfont_mono final: public Font_base {
public:
  explicit Microfont_mono(cr<Str> path);
  ~Microfont_mono();
  void draw(Image& dst, const Veci pos, cr<utf32> text, blend_pf bf=&blend_diff, const int optional=0) const override final;
  inline bool is_mono() const override { return true; }

private:
  struct Impl;
  Unique<Impl> _impl {};
};
