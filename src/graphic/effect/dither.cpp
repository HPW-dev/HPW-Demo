#include <omp.h>
#include <array>
#include <mutex>
#include <algorithm>
#include <unordered_map>
#include "dither.hpp"
#include "util/log.hpp"
#include "util/math/vec.hpp"
#include "graphic/image/image.hpp"
#include "graphic/sprite/sprite.hpp"
#include "graphic/util/graphic-util.hpp"
#include "game/util/sync.hpp"

inline constexpr Vec bayer_16x16_sz {16, 16};
/// bayer 16x16 byte
inline constexpr std::array<byte, scast<std::size_t>(bayer_16x16_sz.x * bayer_16x16_sz.y)> bayer_16x16_byte {
  255, 127, 223, 95 , 247, 119, 215, 87 , 253, 125, 221, 93 , 245, 117, 213, 85 ,
  63 , 191, 31 , 159, 55 , 183, 23 , 151, 61 , 189, 29 , 157, 53 , 181, 21 , 149,
  207, 79 , 239, 111, 199, 71 , 231, 103, 205, 77 , 237, 109, 197, 69 , 229, 101,
  15 , 143, 47 , 175, 7  , 135, 39 , 167, 13 , 141, 45 , 173, 5  , 133, 37 , 165,
  243, 115, 211, 83 , 251, 123, 219, 91 , 241, 113, 209, 81 , 249, 121, 217, 89 ,
  51 , 179, 19 , 147, 59 , 187, 27 , 155, 49 , 177, 17 , 145, 57 , 185, 25 , 153,
  195, 67 , 227, 99 , 203, 75 , 235, 107, 193, 65 , 225, 97 , 201, 73 , 233, 105,
  3  , 131, 35 , 163, 11 , 139, 43 , 171, 1  , 129, 33 , 161, 9  , 137, 41 , 169,
  252, 124, 220, 92 , 244, 116, 212, 84 , 254, 126, 222, 94 , 246, 118, 214, 86 ,
  60 , 188, 28 , 156, 52 , 180, 20 , 148, 62 , 190, 30 , 158, 54 , 182, 22 , 150,
  204, 76 , 236, 108, 196, 68 , 228, 100, 206, 78 , 238, 110, 198, 70 , 230, 102,
  12 , 140, 44 , 172, 4  , 132, 36 , 164, 14 , 142, 46 , 174, 6  , 134, 38 , 166,
  240, 112, 208, 80 , 248, 120, 216, 88 , 242, 114, 210, 82 , 250, 122, 218, 90 ,
  48 , 176, 16 , 144, 56 , 184, 24 , 152, 50 , 178, 18 , 146, 58 , 186, 26 , 154,
  192, 64 , 224, 96 , 200, 72 , 232, 104, 194, 66 , 226, 98 , 202, 74 , 234, 106,
  0  , 128, 32 , 160, 8  , 136, 40 , 168, 2  , 130, 34 , 162, 10 , 138, 42 , 170
};

/// bayer 16x16 real normalized
inline constexpr const std::array<real, scast<std::size_t>(bayer_16x16_sz.x * bayer_16x16_sz.y)> bayer_16x16_real {
  0.496094, -0.253906, 0.308594, -0.441406, 0.449219, -0.300781, 0.261719, -0.488281, 0.484375, -0.265625, 0.296875, -0.453125, 0.4375, -0.3125, 0.25, -0.5,
  -0.00390625, 0.246094, -0.191406, 0.0585938, -0.0507812, 0.199219, -0.238281, 0.0117188, -0.015625, 0.234375, -0.203125, 0.046875, -0.0625, 0.1875, -0.25, 0,
  0.371094, -0.378906, 0.433594, -0.316406, 0.324219, -0.425781, 0.386719, -0.363281, 0.359375, -0.390625, 0.421875, -0.328125, 0.3125, -0.4375, 0.375, -0.375,
  -0.128906, 0.121094, -0.0664062, 0.183594, -0.175781, 0.0742188, -0.113281, 0.136719, -0.140625, 0.109375, -0.078125, 0.171875, -0.1875, 0.0625, -0.125, 0.125,
  0.464844, -0.285156, 0.277344, -0.472656, 0.480469, -0.269531, 0.292969, -0.457031, 0.453125, -0.296875, 0.265625, -0.484375, 0.46875, -0.28125, 0.28125, -0.46875,
  -0.0351562, 0.214844, -0.222656, 0.0273438, -0.0195312, 0.230469, -0.207031, 0.0429688, -0.046875, 0.203125, -0.234375, 0.015625, -0.03125, 0.21875, -0.21875, 0.03125,
  0.339844, -0.410156, 0.402344, -0.347656, 0.355469, -0.394531, 0.417969, -0.332031, 0.328125, -0.421875, 0.390625, -0.359375, 0.34375, -0.40625, 0.40625, -0.34375,
  -0.160156, 0.0898438, -0.0976562, 0.152344, -0.144531, 0.105469, -0.0820312, 0.167969, -0.171875, 0.078125, -0.109375, 0.140625, -0.15625, 0.09375, -0.09375, 0.15625,
  0.488281, -0.261719, 0.300781, -0.449219, 0.441406, -0.308594, 0.253906, -0.496094, 0.492188, -0.257812, 0.304688, -0.445312, 0.445312, -0.304688, 0.257812, -0.492188,
  -0.0117188, 0.238281, -0.199219, 0.0507812, -0.0585938, 0.191406, -0.246094, 0.00390625, -0.0078125, 0.242188, -0.195312, 0.0546875, -0.0546875, 0.195312, -0.242188, 0.0078125,
  0.363281, -0.386719, 0.425781, -0.324219, 0.316406, -0.433594, 0.378906, -0.371094, 0.367188, -0.382812, 0.429688, -0.320312, 0.320312, -0.429688, 0.382812, -0.367188,
  -0.136719, 0.113281, -0.0742188, 0.175781, -0.183594, 0.0664062, -0.121094, 0.128906, -0.132812, 0.117188, -0.0703125, 0.179688, -0.179688, 0.0703125, -0.117188, 0.132812,
  0.457031, -0.292969, 0.269531, -0.480469, 0.472656, -0.277344, 0.285156, -0.464844, 0.460938, -0.289062, 0.273438, -0.476562, 0.476562, -0.273438, 0.289062, -0.460938,
  -0.0429688, 0.207031, -0.230469, 0.0195312, -0.0273438, 0.222656, -0.214844, 0.0351562, -0.0390625, 0.210938, -0.226562, 0.0234375, -0.0234375, 0.226562, -0.210938, 0.0390625,
  0.332031, -0.417969, 0.394531, -0.355469, 0.347656, -0.402344, 0.410156, -0.339844, 0.335938, -0.414062, 0.398438, -0.351562, 0.351562, -0.398438, 0.414062, -0.335938,
  -0.167969, 0.0820312, -0.105469, 0.144531, -0.152344, 0.0976562, -0.0898438, 0.160156, -0.164062, 0.0859375, -0.101562, 0.148438, -0.148438, 0.101562, -0.0859375, 0.16406
};

inline constexpr Vec bayer_2x2_sz {2, 2};
/// bayer 2x2 byte
inline constexpr std::array<byte, scast<std::size_t>(bayer_2x2_sz.x * bayer_2x2_sz.y)> bayer_2x2_byte {
  85, 190,
  255,  0
};

/// bayer 2x2 real normalized
inline constexpr const std::array<real, scast<std::size_t>(bayer_2x2_sz.x * bayer_2x2_sz.y)> bayer_2x2_real {
  -0.25, 0.25,
   0.5,  0.0
};

struct Table {
  Vec sz {};
  CP<real> table {};
};

CN<Table> get_table(Dither type) {
  static const Table null_table {
    .sz = {},
    .table = nullptr
  };
  static std::unordered_map<Dither, Table> map {
    {Dither::bayer16x16, Table{.sz=bayer_16x16_sz, .table=bayer_16x16_real.data()}},
    {Dither::bayer2x2, Table{.sz=bayer_2x2_sz, .table=bayer_2x2_real.data()}},
  };
  try {
    return map.at(type);
  } catch (...) {
    hpw_log("need impl for other tables\n");
    return null_table;
  }
} // get_table

void dither_blend(Image& dst, CN<Image> src, const Vec pos, real alpha,
Dither type, blend_pf bf) {
  error("need impl");
  return_if (!dst || !src);
  return_if (alpha <= 0);
  if (alpha >= 1) {
    insert(dst, src, pos, bf);
    return;
  }
  cfor (y, src.Y)
  cfor (x, src.X) {
    // TODO
  }
}

void dither_blend(Image& dst, CN<Sprite> src, const Vec pos, real alpha,
Dither type, blend_pf bf) {
  return_if (!src || !dst);
  return_if (alpha <= 0);
  if (alpha >= 1) {
    insert(dst, src, pos, bf);
    return;
  }
  auto &mask = *src.get_mask();
  auto &image = *src.get_image();
  cfor (y, src.Y())
  cfor (x, src.X()) {
    // вырезание по маске
    continue_if (mask(x, y) == Pal8::mask_invisible);

    cnauto table = get_table(type);
    auto table_idx = ((y % scast<std::size_t>(table.sz.y)) * scast<std::size_t>(table.sz.x)) + (x % scast<std::size_t>(table.sz.x));
    auto table_mask = table.table[table_idx];
    
    // TODO src/dst table optimization
    if (alpha + table_mask > 0.5)
      dst.set(x + pos.x, y + pos.y, image(x, y), bf, {});
  }
} // dither_blend

inline constexpr real to_4bit(real src) { return scast<int>(src * 4.0) * (1.0 / 4.0); }

static std::once_flag init_table_db16b4_once {};
/// [color][table_x][table_y]
static Pal8 table_db16b4 [256][16][16]{};

/// init precalculated table for dither bayer 16x16 4-bit
inline void init_table_db16b4() {
  const real mul = 1.0 / 4.0; // for 4 bit
  const std::size_t table_x = 16;
  const std::size_t table_y = 16;

  cfor (color, 256)
  cfor (x, table_x)
  cfor (y, table_y) {
    Pal8 pix;
    pix.val = color;
    const bool is_red = pix.is_red();
    real fpix = pix.to_real();
    std::size_t table_idx = (x % table_x) + (y % table_y) * table_y;
    fpix += mul * bayer_16x16_real[table_idx];
    fpix = to_4bit(fpix);
    table_db16b4[color][x][y] = Pal8::from_real(fpix, is_red);
  }
}

inline Pal8 get_table_db16b4(const Pal8 src, std::size_t x, std::size_t y) {
  const std::size_t table_x = 16;
  const std::size_t table_y = 16;
  return table_db16b4[src.val][x % table_x][y % table_y];
}

void fast_dither_bayer16x16_4bit(Image& dst, bool rotate_pattern) {
  return_if (!dst);
  const uint state = rotate_pattern ? graphic::frame_count : 0;
  std::call_once(init_table_db16b4_once, &init_table_db16b4);

  #pragma omp parallel for simd collapse(2)
  cfor (y, scast<uint>(dst.Y))
  cfor (x, scast<uint>(dst.X)) {
    nauto pix = dst(x, y);
    pix = get_table_db16b4(pix, x + state, y + state);
  }
}
