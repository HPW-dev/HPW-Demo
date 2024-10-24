#pragma once
#include "unifont.hpp"

// шрифт для unifont.ttf (моноширинный)
class Unifont_mono final: public Unifont {
  Shared<Unifont::Glyph> _load_glyph(char32_t ch) const override;

public: 
  // @param width if = 0 - use same as height
  explicit Unifont_mono(cr<Str> fname, int width=12, int height=12, bool mono=true);
  // @param width if = 0 - use same as height
  explicit Unifont_mono(cr<File> file, int width=12, int height=12, bool mono=true);
};
