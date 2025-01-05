#include "microfont-mono.hpp"
#include "graphic/image/image.hpp"

struct Microfont_mono::Impl {
  inline Impl(cr<Str> path) {

  }

  inline void draw(Image& dst, const Veci pos, cr<utf32> txt, blend_pf bf, const int opt) const {

  }
};

Microfont_mono::Microfont_mono(cr<Str> path): _impl {new_unique<Impl>(path)} {}
Microfont_mono::~Microfont_mono() {}
void Microfont_mono::draw(Image& dst, const Veci pos, cr<utf32> txt, blend_pf bf, const int opt) const
  { _impl->draw(dst, pos, txt, bf, opt); }
