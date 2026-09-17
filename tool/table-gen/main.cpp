/* Как пользоваться?
g++ -Wall -std=c++26 -pipe -s -O2 -static tool\table-gen\main.cpp -o build\bin\table-gen.exe && build\bin\table-gen.exe
потом идёшь в папку .tmp/ и забираешь таблицы */

#include <algorithm>
#include <stdexcept>
#include <fstream>
#include <cstdint>
#include <cstddef>
#include <utility>
#include <vector>
#include <array>
#include <cmath>

using byte = std::uint8_t;
using bytes = std::vector<byte>;
using uint = unsigned;
using real = double;
using Pal8 = byte;
static_assert(sizeof(Pal8) == 1);

struct Rgb24 { byte r {}, g {}, b {}; };
struct Srgb { real r {}, g {}, b {}; };
struct Irgb { int r {}, g {}, b {}; };

constexpr uint PAL_SZ = 256;
constexpr Pal8 BLACK = 0;
constexpr Pal8 WHITE = 255;
constexpr uint REDS = 32;
constexpr uint GRAYS = PAL_SZ - 1 - REDS;
constexpr Pal8 GRAY_START = BLACK;
constexpr Pal8 GRAY_END = GRAYS - 1;
constexpr Pal8 RED = 254;
constexpr Pal8 RED_START = GRAY_END + 1;
constexpr Pal8 RED_END = RED;

constexpr static const std::array<Rgb24, PAL_SZ> pal8_default_table {
  Rgb24{0, 0, 0},
  Rgb24{1, 1, 1},
  Rgb24{2, 2, 2},
  Rgb24{3, 3, 3},
  Rgb24{5, 5, 5},
  Rgb24{6, 6, 6},
  Rgb24{7, 7, 7},
  Rgb24{8, 8, 8},
  Rgb24{9, 9, 9},
  Rgb24{10, 10, 10},
  Rgb24{11, 11, 11},
  Rgb24{13, 13, 13},
  Rgb24{14, 14, 14},
  Rgb24{15, 15, 15},
  Rgb24{16, 16, 16},
  Rgb24{17, 17, 17},
  Rgb24{18, 18, 18},
  Rgb24{20, 20, 20},
  Rgb24{21, 21, 21},
  Rgb24{22, 22, 22},
  Rgb24{23, 23, 23},
  Rgb24{24, 24, 24},
  Rgb24{25, 25, 25},
  Rgb24{26, 26, 26},
  Rgb24{28, 28, 28},
  Rgb24{29, 29, 29},
  Rgb24{30, 30, 30},
  Rgb24{31, 31, 31},
  Rgb24{32, 32, 32},
  Rgb24{33, 33, 33},
  Rgb24{34, 34, 34},
  Rgb24{36, 36, 36},
  Rgb24{37, 37, 37},
  Rgb24{38, 38, 38},
  Rgb24{39, 39, 39},
  Rgb24{40, 40, 40},
  Rgb24{41, 41, 41},
  Rgb24{43, 43, 43},
  Rgb24{44, 44, 44},
  Rgb24{45, 45, 45},
  Rgb24{46, 46, 46},
  Rgb24{47, 47, 47},
  Rgb24{48, 48, 48},
  Rgb24{49, 49, 49},
  Rgb24{51, 51, 51},
  Rgb24{52, 52, 52},
  Rgb24{53, 53, 53},
  Rgb24{54, 54, 54},
  Rgb24{55, 55, 55},
  Rgb24{56, 56, 56},
  Rgb24{57, 57, 57},
  Rgb24{59, 59, 59},
  Rgb24{60, 60, 60},
  Rgb24{61, 61, 61},
  Rgb24{62, 62, 62},
  Rgb24{63, 63, 63},
  Rgb24{64, 64, 64},
  Rgb24{65, 65, 65},
  Rgb24{67, 67, 67},
  Rgb24{68, 68, 68},
  Rgb24{69, 69, 69},
  Rgb24{70, 70, 70},
  Rgb24{71, 71, 71},
  Rgb24{72, 72, 72},
  Rgb24{74, 74, 74},
  Rgb24{75, 75, 75},
  Rgb24{76, 76, 76},
  Rgb24{77, 77, 77},
  Rgb24{78, 78, 78},
  Rgb24{79, 79, 79},
  Rgb24{80, 80, 80},
  Rgb24{82, 82, 82},
  Rgb24{83, 83, 83},
  Rgb24{84, 84, 84},
  Rgb24{85, 85, 85},
  Rgb24{86, 86, 86},
  Rgb24{87, 87, 87},
  Rgb24{88, 88, 88},
  Rgb24{90, 90, 90},
  Rgb24{91, 91, 91},
  Rgb24{92, 92, 92},
  Rgb24{93, 93, 93},
  Rgb24{94, 94, 94},
  Rgb24{95, 95, 95},
  Rgb24{96, 96, 96},
  Rgb24{98, 98, 98},
  Rgb24{99, 99, 99},
  Rgb24{100, 100, 100},
  Rgb24{101, 101, 101},
  Rgb24{102, 102, 102},
  Rgb24{103, 103, 103},
  Rgb24{105, 105, 105},
  Rgb24{106, 106, 106},
  Rgb24{107, 107, 107},
  Rgb24{108, 108, 108},
  Rgb24{109, 109, 109},
  Rgb24{110, 110, 110},
  Rgb24{111, 111, 111},
  Rgb24{113, 113, 113},
  Rgb24{114, 114, 114},
  Rgb24{115, 115, 115},
  Rgb24{116, 116, 116},
  Rgb24{117, 117, 117},
  Rgb24{118, 118, 118},
  Rgb24{119, 119, 119},
  Rgb24{121, 121, 121},
  Rgb24{122, 122, 122},
  Rgb24{123, 123, 123},
  Rgb24{124, 124, 124},
  Rgb24{125, 125, 125},
  Rgb24{126, 126, 126},
  Rgb24{128, 128, 128},
  Rgb24{129, 129, 129},
  Rgb24{130, 130, 130},
  Rgb24{131, 131, 131},
  Rgb24{132, 132, 132},
  Rgb24{133, 133, 133},
  Rgb24{134, 134, 134},
  Rgb24{136, 136, 136},
  Rgb24{137, 137, 137},
  Rgb24{138, 138, 138},
  Rgb24{139, 139, 139},
  Rgb24{140, 140, 140},
  Rgb24{141, 141, 141},
  Rgb24{142, 142, 142},
  Rgb24{144, 144, 144},
  Rgb24{145, 145, 145},
  Rgb24{146, 146, 146},
  Rgb24{147, 147, 147},
  Rgb24{148, 148, 148},
  Rgb24{149, 149, 149},
  Rgb24{150, 150, 150},
  Rgb24{152, 152, 152},
  Rgb24{153, 153, 153},
  Rgb24{154, 154, 154},
  Rgb24{155, 155, 155},
  Rgb24{156, 156, 156},
  Rgb24{157, 157, 157},
  Rgb24{159, 159, 159},
  Rgb24{160, 160, 160},
  Rgb24{161, 161, 161},
  Rgb24{162, 162, 162},
  Rgb24{163, 163, 163},
  Rgb24{164, 164, 164},
  Rgb24{165, 165, 165},
  Rgb24{167, 167, 167},
  Rgb24{168, 168, 168},
  Rgb24{169, 169, 169},
  Rgb24{170, 170, 170},
  Rgb24{171, 171, 171},
  Rgb24{172, 172, 172},
  Rgb24{173, 173, 173},
  Rgb24{175, 175, 175},
  Rgb24{176, 176, 176},
  Rgb24{177, 177, 177},
  Rgb24{178, 178, 178},
  Rgb24{179, 179, 179},
  Rgb24{180, 180, 180},
  Rgb24{181, 181, 181},
  Rgb24{183, 183, 183},
  Rgb24{184, 184, 184},
  Rgb24{185, 185, 185},
  Rgb24{186, 186, 186},
  Rgb24{187, 187, 187},
  Rgb24{188, 188, 188},
  Rgb24{190, 190, 190},
  Rgb24{191, 191, 191},
  Rgb24{192, 192, 192},
  Rgb24{193, 193, 193},
  Rgb24{194, 194, 194},
  Rgb24{195, 195, 195},
  Rgb24{196, 196, 196},
  Rgb24{198, 198, 198},
  Rgb24{199, 199, 199},
  Rgb24{200, 200, 200},
  Rgb24{201, 201, 201},
  Rgb24{202, 202, 202},
  Rgb24{203, 203, 203},
  Rgb24{204, 204, 204},
  Rgb24{206, 206, 206},
  Rgb24{207, 207, 207},
  Rgb24{208, 208, 208},
  Rgb24{209, 209, 209},
  Rgb24{210, 210, 210},
  Rgb24{211, 211, 211},
  Rgb24{213, 213, 213},
  Rgb24{214, 214, 214},
  Rgb24{215, 215, 215},
  Rgb24{216, 216, 216},
  Rgb24{217, 217, 217},
  Rgb24{218, 218, 218},
  Rgb24{219, 219, 219},
  Rgb24{221, 221, 221},
  Rgb24{222, 222, 222},
  Rgb24{223, 223, 223},
  Rgb24{224, 224, 224},
  Rgb24{225, 225, 225},
  Rgb24{226, 226, 226},
  Rgb24{227, 227, 227},
  Rgb24{229, 229, 229},
  Rgb24{230, 230, 230},
  Rgb24{231, 231, 231},
  Rgb24{232, 232, 232},
  Rgb24{233, 233, 233},
  Rgb24{234, 234, 234},
  Rgb24{235, 235, 235},
  Rgb24{237, 237, 237},
  Rgb24{238, 238, 238},
  Rgb24{239, 239, 239},
  Rgb24{240, 240, 240},
  Rgb24{241, 241, 241},
  Rgb24{242, 242, 242},
  Rgb24{244, 244, 244},
  Rgb24{245, 245, 245},
  Rgb24{246, 246, 246},
  Rgb24{247, 247, 247},
  Rgb24{248, 248, 248},
  Rgb24{249, 249, 249},
  Rgb24{250, 250, 250},
  Rgb24{252, 252, 252},
  Rgb24{253, 253, 253},
  Rgb24{254, 254, 254},
  Rgb24{255, 255, 255},
  Rgb24{8, 0, 0},
  Rgb24{16, 0, 0},
  Rgb24{24, 0, 0},
  Rgb24{30, 0, 0},
  Rgb24{36, 0, 0},
  Rgb24{40, 0, 0},
  Rgb24{45, 0, 0},
  Rgb24{51, 0, 0},
  Rgb24{57, 0, 0},
  Rgb24{64, 0, 0},
  Rgb24{70, 0, 0},
  Rgb24{77, 0, 0},
  Rgb24{84, 0, 0},
  Rgb24{92, 0, 0},
  Rgb24{99, 0, 0},
  Rgb24{107, 0, 0},
  Rgb24{115, 0, 0},
  Rgb24{123, 0, 0},
  Rgb24{131, 0, 0},
  Rgb24{139, 0, 0},
  Rgb24{148, 0, 0},
  Rgb24{157, 0, 0},
  Rgb24{166, 0, 0},
  Rgb24{175, 0, 0},
  Rgb24{185, 0, 0},
  Rgb24{194, 0, 0},
  Rgb24{204, 0, 0},
  Rgb24{214, 0, 0},
  Rgb24{224, 0, 0},
  Rgb24{234, 0, 0},
  Rgb24{245, 0, 0},
  Rgb24{255, 0, 0},
  Rgb24{255, 255, 255}
}; // pal8_default_table

#define USE_GAMMA_CORR

static inline real linear_to_srgb(real c) {
#ifdef USE_GAMMA_CORR
  return (c <= 0.0031308)
    ? (12.92 * c)
    : (1.055 * std::pow(c, 1.0 / 2.4) - 0.055);
#else
  return c;
#endif
}

static inline real srgb_to_linear(real c) {
#ifdef USE_GAMMA_CORR
  return (c <= 0.04045)
    ? (c / 12.92)
    : (std::pow((c + 0.055) / 1.055, 2.4));
#else
  return c;
#endif
}

static inline void error_if(bool cond_for_error, const char* msg) {
  if (cond_for_error)
    throw std::runtime_error(msg);
}

static inline real blend(real a, real b, real alpha) {
  return a + (b - a) * alpha;
}

Pal8 rgb24_to_pal8(Rgb24 src) {
  real min_dist = 999'999'999;
  Pal8 result = 0;
  
  const int sr = static_cast<int>(src.r);
  const int sg = static_cast<int>(src.g);
  const int sb = static_cast<int>(src.b);

  for (size_t i = 0; i < pal8_default_table.size(); ++i) {
    const auto pal = pal8_default_table[i];
    const int dr = pal.r - sr;
    const int dg = pal.g - sg;
    const int db = pal.b - sb;
    const real dist =
      dr * dr * 0.296952 +
      dg * dg * 0.586612 +
      db * db * 0.114436;
    
    if (dist < min_dist) {
      min_dist = dist;
      result = static_cast<Pal8>(i);
    }
  }

  return result;
}

Pal8 srgb_to_pal8(Srgb src) {
  const Rgb24 rgb24 {
    .r = static_cast<byte>(std::clamp<real>(std::round(linear_to_srgb(src.r) * 255.0), 0.0, 255.0)),
    .g = static_cast<byte>(std::clamp<real>(std::round(linear_to_srgb(src.g) * 255.0), 0.0, 255.0)),
    .b = static_cast<byte>(std::clamp<real>(std::round(linear_to_srgb(src.b) * 255.0), 0.0, 255.0)),
  };
  return rgb24_to_pal8(rgb24);
}

Srgb pal8_to_srgb(Pal8 src) {
  const auto rgb24 = pal8_default_table.at(src);
  return Srgb {
    .r = srgb_to_linear(rgb24.r / 255.0),
    .g = srgb_to_linear(rgb24.g / 255.0),
    .b = srgb_to_linear(rgb24.b / 255.0),
  };
}

Rgb24 pal8_to_rgb24(Pal8 src) {
  return pal8_default_table.at(src);
}

void save(const char* name, const bytes& table) {
  error_if(!name, "file name is empty");
  error_if(table.empty(), "table data is empty");
  error_if(table.size() < PAL_SZ, "table size < 256");

  auto file = std::ofstream(name, std::ios_base::binary);
  error_if(!file.is_open(), "file not opened");
  
  file.write(reinterpret_cast<const char*>(table.data()), table.size());
}

bytes make_inv() {
  bytes table;
  for (uint c = 0; c < PAL_SZ; ++c)
    table.push_back(~byte(c));
  return table;
}

bytes make_dec_safe() {
  bytes table;
  for (uint i = 0; i < PAL_SZ; ++i) {
    auto rgb24 = pal8_to_rgb24(static_cast<Pal8>(i));
    rgb24.r = std::clamp<int>(int(rgb24.r) - 1, 0, 255);
    rgb24.g = std::clamp<int>(int(rgb24.g) - 1, 0, 255);
    rgb24.b = std::clamp<int>(int(rgb24.b) - 1, 0, 255);
    table.push_back(rgb24_to_pal8(rgb24));
  }
  return table;
}

bytes make_inc_safe() {
  bytes table;
  for (uint i = 0; i < PAL_SZ; ++i) {
    auto rgb24 = pal8_to_rgb24(static_cast<Pal8>(i));
    rgb24.r = std::clamp<int>(int(rgb24.r) + 1, 0, 255);
    rgb24.g = std::clamp<int>(int(rgb24.g) + 1, 0, 255);
    rgb24.b = std::clamp<int>(int(rgb24.b) + 1, 0, 255);
    table.push_back(rgb24_to_pal8(rgb24));
  }
  return table;
}

bytes make_inv_safe() {
  bytes table;
  for (int c = GRAYS - 1; c > -1; --c) table.push_back(c);
  for (int c = RED_END; c > RED_START-1; --c) table.push_back(c);
  table.push_back(BLACK);
  return table;
}

bytes make_add() {
  bytes table;
  for (uint a = 0; a < PAL_SZ; ++a)
  for (uint b = 0; b < PAL_SZ; ++b)
    table.push_back(byte(a + b));
  return table;
}

bytes make_add_safe() {
  bytes table;
  for (uint b = 0; b < PAL_SZ; ++b)
  for (uint a = 0; a < PAL_SZ; ++a) {
          auto a_rgb = pal8_to_rgb24(static_cast<Pal8>(a));
    const auto b_rgb = pal8_to_rgb24(static_cast<Pal8>(b));
    a_rgb.r = std::clamp<int>(int(a_rgb.r) + b_rgb.r, 0, 255);
    a_rgb.g = std::clamp<int>(int(a_rgb.g) + b_rgb.g, 0, 255);
    a_rgb.b = std::clamp<int>(int(a_rgb.b) + b_rgb.b, 0, 255);
    table.push_back(rgb24_to_pal8(a_rgb));
  }
  return table;
}

bytes make_sub() {
  bytes table;
  for (uint a = 0; a < PAL_SZ; ++a)
  for (uint b = 0; b < PAL_SZ; ++b)
    table.push_back(byte(a - b));
  return table;
}

bytes make_sub_safe() {
  bytes table;
  for (uint b = 0; b < PAL_SZ; ++b)
  for (uint a = 0; a < PAL_SZ; ++a) {
          auto a_rgb = pal8_to_rgb24(static_cast<Pal8>(a));
    const auto b_rgb = pal8_to_rgb24(static_cast<Pal8>(b));
    a_rgb.r = std::clamp<int>(int(a_rgb.r) - b_rgb.r, 0, 255);
    a_rgb.g = std::clamp<int>(int(a_rgb.g) - b_rgb.g, 0, 255);
    a_rgb.b = std::clamp<int>(int(a_rgb.b) - b_rgb.b, 0, 255);
    table.push_back(rgb24_to_pal8(a_rgb));
  }
  return table;
}

bytes make_and() {
  bytes table;
  for (uint b = 0; b < PAL_SZ; ++b)
  for (uint a = 0; a < PAL_SZ; ++a)
    table.push_back(byte(a & b));
  return table;
}

bytes make_and_safe() {
  bytes table;
  for (uint b = 0; b < PAL_SZ; ++b)
  for (uint a = 0; a < PAL_SZ; ++a) {
          auto a_rgb = pal8_to_rgb24(static_cast<Pal8>(a));
    const auto b_rgb = pal8_to_rgb24(static_cast<Pal8>(b));
    a_rgb.r = a_rgb.r & b_rgb.r;
    a_rgb.g = a_rgb.g & b_rgb.g;
    a_rgb.b = a_rgb.b & b_rgb.b;
    table.push_back(rgb24_to_pal8(a_rgb));
  }
  return table;
}

bytes make_or() {
  bytes table;
  for (uint b = 0; b < PAL_SZ; ++b)
  for (uint a = 0; a < PAL_SZ; ++a)
    table.push_back(byte(a | b));
  return table;
}

bytes make_or_safe() {
  bytes table;
  for (uint b = 0; b < PAL_SZ; ++b)
  for (uint a = 0; a < PAL_SZ; ++a) {
          auto a_rgb = pal8_to_rgb24(static_cast<Pal8>(a));
    const auto b_rgb = pal8_to_rgb24(static_cast<Pal8>(b));
    a_rgb.r = a_rgb.r | b_rgb.r;
    a_rgb.g = a_rgb.g | b_rgb.g;
    a_rgb.b = a_rgb.b | b_rgb.b;
    table.push_back(rgb24_to_pal8(a_rgb));
  }
  return table;
}

bytes make_mul() {
  bytes table;
  for (uint b = 0; b < PAL_SZ; ++b)
  for (uint a = 0; a < PAL_SZ; ++a)
    table.push_back(byte(a * b));
  return table;
}

bytes make_mul_safe() {
  bytes table;
  for (uint b = 0; b < PAL_SZ; ++b)
  for (uint a = 0; a < PAL_SZ; ++a) {
          auto a_rgb = pal8_to_srgb(static_cast<Pal8>(a));
    const auto b_rgb = pal8_to_srgb(static_cast<Pal8>(b));
    a_rgb.r = a_rgb.r * b_rgb.r;
    a_rgb.g = a_rgb.g * b_rgb.g;
    a_rgb.b = a_rgb.b * b_rgb.b;
    table.push_back(srgb_to_pal8(a_rgb));
  }
  return table;
}

bytes make_xor() {
  bytes table;
  for (uint a = 0; a < PAL_SZ; ++a)
  for (uint b = 0; b < PAL_SZ; ++b)
    table.push_back(byte(a ^ b));
  return table;
}

bytes make_xor_safe() {
  bytes table;
  for (uint b = 0; b < PAL_SZ; ++b)
  for (uint a = 0; a < PAL_SZ; ++a) {
          auto a_rgb = pal8_to_rgb24(static_cast<Pal8>(a));
    const auto b_rgb = pal8_to_rgb24(static_cast<Pal8>(b));
    a_rgb.r = a_rgb.r ^ b_rgb.r;
    a_rgb.g = a_rgb.g ^ b_rgb.g;
    a_rgb.b = a_rgb.b ^ b_rgb.b;
    table.push_back(rgb24_to_pal8(a_rgb));
  }
  return table;
}

bytes make_diff() {
  bytes table;
  for (uint b = 0; b < PAL_SZ; ++b)
  for (uint a = 0; a < PAL_SZ; ++a) {
          auto a_rgb = pal8_to_rgb24(static_cast<Pal8>(a));
    const auto b_rgb = pal8_to_rgb24(static_cast<Pal8>(b));
    a_rgb.r = std::abs(int(a_rgb.r) - b_rgb.r);
    a_rgb.g = std::abs(int(a_rgb.g) - b_rgb.g);
    a_rgb.b = std::abs(int(a_rgb.b) - b_rgb.b);
    table.push_back(rgb24_to_pal8(a_rgb));
  }
  return table;
}

bytes make_avr() {
  bytes table;
  for (uint b = 0; b < PAL_SZ; ++b)
  for (uint a = 0; a < PAL_SZ; ++a) {
          auto a_rgb = pal8_to_srgb(static_cast<Pal8>(a));
    const auto b_rgb = pal8_to_srgb(static_cast<Pal8>(b));
    a_rgb.r = (a_rgb.r + b_rgb.r) * 0.5;
    a_rgb.g = (a_rgb.g + b_rgb.g) * 0.5;
    a_rgb.b = (a_rgb.b + b_rgb.b) * 0.5;
    table.push_back(srgb_to_pal8(a_rgb));
  }
  return table;
}

bytes make_avr_max() {
  bytes table;
  for (uint b = 0; b < PAL_SZ; ++b)
  for (uint a = 0; a < PAL_SZ; ++a) {
          auto a_rgb = pal8_to_srgb(static_cast<Pal8>(a));
    const auto b_rgb = pal8_to_srgb(static_cast<Pal8>(b));
    a_rgb.r = std::max<real>((a_rgb.r + b_rgb.r) * 0.5, b_rgb.r);
    a_rgb.g = std::max<real>((a_rgb.g + b_rgb.g) * 0.5, b_rgb.g);
    a_rgb.b = std::max<real>((a_rgb.b + b_rgb.b) * 0.5, b_rgb.b);
    table.push_back(srgb_to_pal8(a_rgb));
  }
  return table;
}

bytes make_blend_readable() {
  bytes table;
  constexpr real alpha = 1.0 - (43.0 / 255.0);
  for (uint b = 0; b < PAL_SZ; ++b)
  for (uint a = 0; a < PAL_SZ; ++a) {
          auto a_rgb = pal8_to_srgb(static_cast<Pal8>(a));
    const auto b_rgb = pal8_to_srgb(static_cast<Pal8>(b));
    a_rgb.r = blend(a_rgb.r, b_rgb.r, alpha);
    a_rgb.g = blend(a_rgb.g, b_rgb.g, alpha);
    a_rgb.b = blend(a_rgb.b, b_rgb.b, alpha);
    table.push_back(srgb_to_pal8(a_rgb));
  }
  return table;
}

bytes make_max() {
  bytes table;
  for (uint b = 0; b < PAL_SZ; ++b)
  for (uint a = 0; a < PAL_SZ; ++a) {
          auto a_rgb = pal8_to_rgb24(static_cast<Pal8>(a));
    const auto b_rgb = pal8_to_rgb24(static_cast<Pal8>(b));
    a_rgb.r = std::max(a_rgb.r, b_rgb.r);
    a_rgb.g = std::max(a_rgb.g, b_rgb.g);
    a_rgb.b = std::max(a_rgb.b, b_rgb.b);
    table.push_back(rgb24_to_pal8(a_rgb));
  }
  return table;
}

bytes make_min() {
  bytes table;
  for (uint b = 0; b < PAL_SZ; ++b)
  for (uint a = 0; a < PAL_SZ; ++a) {
          auto a_rgb = pal8_to_rgb24(static_cast<Pal8>(a));
    const auto b_rgb = pal8_to_rgb24(static_cast<Pal8>(b));
    a_rgb.r = std::min(a_rgb.r, b_rgb.r);
    a_rgb.g = std::min(a_rgb.g, b_rgb.g);
    a_rgb.b = std::min(a_rgb.b, b_rgb.b);
    table.push_back(rgb24_to_pal8(a_rgb));
  }
  return table;
}

bytes make_overlay() {
  bytes table;
  #define OVERLAY(A, B) (A < 0.5f) ? \
      (2 * A * B) : \
      (1.0 - 2 * (1.0 - A) * (1.0 - B))
  for (uint b = 0; b < PAL_SZ; ++b)
  for (uint a = 0; a < PAL_SZ; ++a) {
          auto a_rgb = pal8_to_rgb24(static_cast<Pal8>(a));
    const auto b_rgb = pal8_to_rgb24(static_cast<Pal8>(b));
    a_rgb.r = OVERLAY(a_rgb.r/255.0, b_rgb.r/255.0) * 255.0;
    a_rgb.g = OVERLAY(a_rgb.g/255.0, b_rgb.g/255.0) * 255.0;
    a_rgb.b = OVERLAY(a_rgb.b/255.0, b_rgb.b/255.0) * 255.0;
    table.push_back(rgb24_to_pal8(a_rgb));
  }
  #undef OVERLAY
  return table;
}

bytes make_softlight() {
  bytes table;
  #define SOFTLIGHT(A, B) (A + (2.0 * B * (A * (1.0 - A))))
  for (uint b = 0; b < PAL_SZ; ++b)
  for (uint a = 0; a < PAL_SZ; ++a) {
          auto a_rgb = pal8_to_rgb24(static_cast<Pal8>(a));
    const auto b_rgb = pal8_to_rgb24(static_cast<Pal8>(b));
    a_rgb.r = SOFTLIGHT(a_rgb.r/255.0, b_rgb.r/255.0) * 255.0;
    a_rgb.g = SOFTLIGHT(a_rgb.g/255.0, b_rgb.g/255.0) * 255.0;
    a_rgb.b = SOFTLIGHT(a_rgb.b/255.0, b_rgb.b/255.0) * 255.0;
    table.push_back(rgb24_to_pal8(a_rgb));
  }
  #undef SOFTLIGHT
  return table;
}

bytes make_fade_out_max() {
  bytes table;
  for (uint o = 0; o < PAL_SZ; ++o)
  for (uint b = 0; b < PAL_SZ; ++b)
  for (uint a = 0; a < PAL_SZ; ++a) {
          auto a_rgb = pal8_to_rgb24(static_cast<Pal8>(a));
    const auto b_rgb = pal8_to_rgb24(static_cast<Pal8>(b));
    a_rgb.r = std::max<int>(int(a_rgb.r) - o, b_rgb.r);
    a_rgb.g = std::max<int>(int(a_rgb.g) - o, b_rgb.g);
    a_rgb.b = std::max<int>(int(a_rgb.b) - o, b_rgb.b);
    table.push_back(rgb24_to_pal8(a_rgb));
  }
  return table;
}

bytes make_fade_in_max() {
  bytes table;
  for (uint o = 0; o < PAL_SZ; ++o)
  for (uint b = 0; b < PAL_SZ; ++b)
  for (uint a = 0; a < PAL_SZ; ++a) {
          auto a_rgb = pal8_to_rgb24(static_cast<Pal8>(a));
    const auto b_rgb = pal8_to_rgb24(static_cast<Pal8>(b));
    const real option = 255 - o;
    a_rgb.r = std::max<int>(int(a_rgb.r) - option, b_rgb.r);
    a_rgb.g = std::max<int>(int(a_rgb.g) - option, b_rgb.g);
    a_rgb.b = std::max<int>(int(a_rgb.b) - option, b_rgb.b);
    table.push_back(rgb24_to_pal8(a_rgb));
  }
  return table;
}

bytes make_blend_alpha() {
  bytes table;
  for (uint o = 0; o < PAL_SZ; ++o)
  for (uint b = 0; b < PAL_SZ; ++b)
  for (uint a = 0; a < PAL_SZ; ++a) {
          auto a_rgb = pal8_to_srgb(static_cast<Pal8>(a));
    const auto b_rgb = pal8_to_srgb(static_cast<Pal8>(b));
    const real option = o / 255.0;
    a_rgb.r = blend(a_rgb.r, b_rgb.r, option);
    a_rgb.g = blend(a_rgb.g, b_rgb.g, option);
    a_rgb.b = blend(a_rgb.b, b_rgb.b, option);
    table.push_back(srgb_to_pal8(a_rgb));
  }
  return table;
}

int main() {
  save(".tmp/table_inv.dat", make_inv());
  save(".tmp/table_dec_safe.dat", make_dec_safe());
  save(".tmp/table_inc_safe.dat", make_inc_safe());
  save(".tmp/table_inv_safe.dat", make_inv_safe());
  save(".tmp/table_add.dat", make_add());
  save(".tmp/table_add_safe.dat", make_add_safe());
  save(".tmp/table_sub.dat", make_sub());
  save(".tmp/table_sub_safe.dat", make_sub_safe());
  save(".tmp/table_and.dat", make_and());
  save(".tmp/table_and_safe.dat", make_and_safe());
  save(".tmp/table_or.dat", make_or());
  save(".tmp/table_or_safe.dat", make_or_safe());
  save(".tmp/table_mul.dat", make_mul());
  save(".tmp/table_mul_safe.dat", make_mul_safe());
  save(".tmp/table_xor.dat", make_xor());
  save(".tmp/table_xor_safe.dat", make_xor_safe());
  save(".tmp/table_diff.dat", make_diff());
  save(".tmp/table_avr.dat", make_avr());
  save(".tmp/table_avr_max.dat", make_avr_max());
  save(".tmp/table_blend_readable.dat", make_blend_readable());
  save(".tmp/table_max.dat", make_max());
  save(".tmp/table_min.dat", make_min());
  save(".tmp/table_overlay.dat", make_overlay());
  save(".tmp/table_softlight.dat", make_softlight());
  save(".tmp/table_fade_out_max.dat", make_fade_out_max());
  save(".tmp/table_fade_in_max.dat", make_fade_in_max());
  save(".tmp/table_blend_alpha.dat", make_blend_alpha());
  return EXIT_SUCCESS;
}
