#pragma once
#include "util/str.hpp"
#include "util/macro.hpp"
#include "util/math/num-types.hpp"
#include "graphic/image/image-fwd.hpp"

enum class Img_to_asci_mode {
  grayscale_large_pal,
  grayscale_small_pal,
  grayscale_small_pal_2,
};

// превращает картинки в ASCI арты
[[nodiscard]] Str to_asci(cr<Image> src, const uint W=80, const uint H=24,
  const Img_to_asci_mode MODE=Img_to_asci_mode::grayscale_large_pal);
