#include <cassert>
#include "blend.hpp"
#include "graphic/image/image.hpp"

void accept_blend_mask_fast(Image& dst, cr<Image> src, cr<Image> blend_mask) {
  assert(dst);
  assert(src);
  assert(blend_mask);
  assert((dst.size == src.size) && (dst.size == blend_mask.size));

  cfor (i, blend_mask.size)
    dst[i] = blend_alpha(src[i], dst[i], scast<int>(blend_mask[i].val));
}
