#pragma once
#include "unifont.hpp"

// шрифт для unifont.ttf (моноширинный)
class Unifont_mono final: public Unifont {
  Shared<Unifont::Glyph> _load_glyph(char32_t ch) const override;

public:
  inline explicit Unifont_mono(cr<Str> fname, int height=12, bool mono=true): Unifont(fname, height, mono) {}
  inline explicit Unifont_mono(cr<File> file, int height=12, bool mono=true): Unifont(file, height, mono) {}
};
