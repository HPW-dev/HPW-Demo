#include <array>
#include "palette.hpp"
#include "color.hpp"

constexpr static const std::array<Rgb24, 256> pal8_default_table {
  Rgb24{0, 0, 0, {}},
  Rgb24{1, 1, 1, {}},
  Rgb24{2, 2, 2, {}},
  Rgb24{3, 3, 3, {}},
  Rgb24{5, 5, 5, {}},
  Rgb24{6, 6, 6, {}},
  Rgb24{7, 7, 7, {}},
  Rgb24{8, 8, 8, {}},
  Rgb24{9, 9, 9, {}},
  Rgb24{10, 10, 10, {}},
  Rgb24{11, 11, 11, {}},
  Rgb24{13, 13, 13, {}},
  Rgb24{14, 14, 14, {}},
  Rgb24{15, 15, 15, {}},
  Rgb24{16, 16, 16, {}},
  Rgb24{17, 17, 17, {}},
  Rgb24{18, 18, 18, {}},
  Rgb24{20, 20, 20, {}},
  Rgb24{21, 21, 21, {}},
  Rgb24{22, 22, 22, {}},
  Rgb24{23, 23, 23, {}},
  Rgb24{24, 24, 24, {}},
  Rgb24{25, 25, 25, {}},
  Rgb24{26, 26, 26, {}},
  Rgb24{28, 28, 28, {}},
  Rgb24{29, 29, 29, {}},
  Rgb24{30, 30, 30, {}},
  Rgb24{31, 31, 31, {}},
  Rgb24{32, 32, 32, {}},
  Rgb24{33, 33, 33, {}},
  Rgb24{34, 34, 34, {}},
  Rgb24{36, 36, 36, {}},
  Rgb24{37, 37, 37, {}},
  Rgb24{38, 38, 38, {}},
  Rgb24{39, 39, 39, {}},
  Rgb24{40, 40, 40, {}},
  Rgb24{41, 41, 41, {}},
  Rgb24{43, 43, 43, {}},
  Rgb24{44, 44, 44, {}},
  Rgb24{45, 45, 45, {}},
  Rgb24{46, 46, 46, {}},
  Rgb24{47, 47, 47, {}},
  Rgb24{48, 48, 48, {}},
  Rgb24{49, 49, 49, {}},
  Rgb24{51, 51, 51, {}},
  Rgb24{52, 52, 52, {}},
  Rgb24{53, 53, 53, {}},
  Rgb24{54, 54, 54, {}},
  Rgb24{55, 55, 55, {}},
  Rgb24{56, 56, 56, {}},
  Rgb24{57, 57, 57, {}},
  Rgb24{59, 59, 59, {}},
  Rgb24{60, 60, 60, {}},
  Rgb24{61, 61, 61, {}},
  Rgb24{62, 62, 62, {}},
  Rgb24{63, 63, 63, {}},
  Rgb24{64, 64, 64, {}},
  Rgb24{65, 65, 65, {}},
  Rgb24{67, 67, 67, {}},
  Rgb24{68, 68, 68, {}},
  Rgb24{69, 69, 69, {}},
  Rgb24{70, 70, 70, {}},
  Rgb24{71, 71, 71, {}},
  Rgb24{72, 72, 72, {}},
  Rgb24{74, 74, 74, {}},
  Rgb24{75, 75, 75, {}},
  Rgb24{76, 76, 76, {}},
  Rgb24{77, 77, 77, {}},
  Rgb24{78, 78, 78, {}},
  Rgb24{79, 79, 79, {}},
  Rgb24{80, 80, 80, {}},
  Rgb24{82, 82, 82, {}},
  Rgb24{83, 83, 83, {}},
  Rgb24{84, 84, 84, {}},
  Rgb24{85, 85, 85, {}},
  Rgb24{86, 86, 86, {}},
  Rgb24{87, 87, 87, {}},
  Rgb24{88, 88, 88, {}},
  Rgb24{90, 90, 90, {}},
  Rgb24{91, 91, 91, {}},
  Rgb24{92, 92, 92, {}},
  Rgb24{93, 93, 93, {}},
  Rgb24{94, 94, 94, {}},
  Rgb24{95, 95, 95, {}},
  Rgb24{96, 96, 96, {}},
  Rgb24{98, 98, 98, {}},
  Rgb24{99, 99, 99, {}},
  Rgb24{100, 100, 100, {}},
  Rgb24{101, 101, 101, {}},
  Rgb24{102, 102, 102, {}},
  Rgb24{103, 103, 103, {}},
  Rgb24{105, 105, 105, {}},
  Rgb24{106, 106, 106, {}},
  Rgb24{107, 107, 107, {}},
  Rgb24{108, 108, 108, {}},
  Rgb24{109, 109, 109, {}},
  Rgb24{110, 110, 110, {}},
  Rgb24{111, 111, 111, {}},
  Rgb24{113, 113, 113, {}},
  Rgb24{114, 114, 114, {}},
  Rgb24{115, 115, 115, {}},
  Rgb24{116, 116, 116, {}},
  Rgb24{117, 117, 117, {}},
  Rgb24{118, 118, 118, {}},
  Rgb24{119, 119, 119, {}},
  Rgb24{121, 121, 121, {}},
  Rgb24{122, 122, 122, {}},
  Rgb24{123, 123, 123, {}},
  Rgb24{124, 124, 124, {}},
  Rgb24{125, 125, 125, {}},
  Rgb24{126, 126, 126, {}},
  Rgb24{128, 128, 128, {}},
  Rgb24{129, 129, 129, {}},
  Rgb24{130, 130, 130, {}},
  Rgb24{131, 131, 131, {}},
  Rgb24{132, 132, 132, {}},
  Rgb24{133, 133, 133, {}},
  Rgb24{134, 134, 134, {}},
  Rgb24{136, 136, 136, {}},
  Rgb24{137, 137, 137, {}},
  Rgb24{138, 138, 138, {}},
  Rgb24{139, 139, 139, {}},
  Rgb24{140, 140, 140, {}},
  Rgb24{141, 141, 141, {}},
  Rgb24{142, 142, 142, {}},
  Rgb24{144, 144, 144, {}},
  Rgb24{145, 145, 145, {}},
  Rgb24{146, 146, 146, {}},
  Rgb24{147, 147, 147, {}},
  Rgb24{148, 148, 148, {}},
  Rgb24{149, 149, 149, {}},
  Rgb24{150, 150, 150, {}},
  Rgb24{152, 152, 152, {}},
  Rgb24{153, 153, 153, {}},
  Rgb24{154, 154, 154, {}},
  Rgb24{155, 155, 155, {}},
  Rgb24{156, 156, 156, {}},
  Rgb24{157, 157, 157, {}},
  Rgb24{159, 159, 159, {}},
  Rgb24{160, 160, 160, {}},
  Rgb24{161, 161, 161, {}},
  Rgb24{162, 162, 162, {}},
  Rgb24{163, 163, 163, {}},
  Rgb24{164, 164, 164, {}},
  Rgb24{165, 165, 165, {}},
  Rgb24{167, 167, 167, {}},
  Rgb24{168, 168, 168, {}},
  Rgb24{169, 169, 169, {}},
  Rgb24{170, 170, 170, {}},
  Rgb24{171, 171, 171, {}},
  Rgb24{172, 172, 172, {}},
  Rgb24{173, 173, 173, {}},
  Rgb24{175, 175, 175, {}},
  Rgb24{176, 176, 176, {}},
  Rgb24{177, 177, 177, {}},
  Rgb24{178, 178, 178, {}},
  Rgb24{179, 179, 179, {}},
  Rgb24{180, 180, 180, {}},
  Rgb24{181, 181, 181, {}},
  Rgb24{183, 183, 183, {}},
  Rgb24{184, 184, 184, {}},
  Rgb24{185, 185, 185, {}},
  Rgb24{186, 186, 186, {}},
  Rgb24{187, 187, 187, {}},
  Rgb24{188, 188, 188, {}},
  Rgb24{190, 190, 190, {}},
  Rgb24{191, 191, 191, {}},
  Rgb24{192, 192, 192, {}},
  Rgb24{193, 193, 193, {}},
  Rgb24{194, 194, 194, {}},
  Rgb24{195, 195, 195, {}},
  Rgb24{196, 196, 196, {}},
  Rgb24{198, 198, 198, {}},
  Rgb24{199, 199, 199, {}},
  Rgb24{200, 200, 200, {}},
  Rgb24{201, 201, 201, {}},
  Rgb24{202, 202, 202, {}},
  Rgb24{203, 203, 203, {}},
  Rgb24{204, 204, 204, {}},
  Rgb24{206, 206, 206, {}},
  Rgb24{207, 207, 207, {}},
  Rgb24{208, 208, 208, {}},
  Rgb24{209, 209, 209, {}},
  Rgb24{210, 210, 210, {}},
  Rgb24{211, 211, 211, {}},
  Rgb24{213, 213, 213, {}},
  Rgb24{214, 214, 214, {}},
  Rgb24{215, 215, 215, {}},
  Rgb24{216, 216, 216, {}},
  Rgb24{217, 217, 217, {}},
  Rgb24{218, 218, 218, {}},
  Rgb24{219, 219, 219, {}},
  Rgb24{221, 221, 221, {}},
  Rgb24{222, 222, 222, {}},
  Rgb24{223, 223, 223, {}},
  Rgb24{224, 224, 224, {}},
  Rgb24{225, 225, 225, {}},
  Rgb24{226, 226, 226, {}},
  Rgb24{227, 227, 227, {}},
  Rgb24{229, 229, 229, {}},
  Rgb24{230, 230, 230, {}},
  Rgb24{231, 231, 231, {}},
  Rgb24{232, 232, 232, {}},
  Rgb24{233, 233, 233, {}},
  Rgb24{234, 234, 234, {}},
  Rgb24{235, 235, 235, {}},
  Rgb24{237, 237, 237, {}},
  Rgb24{238, 238, 238, {}},
  Rgb24{239, 239, 239, {}},
  Rgb24{240, 240, 240, {}},
  Rgb24{241, 241, 241, {}},
  Rgb24{242, 242, 242, {}},
  Rgb24{244, 244, 244, {}},
  Rgb24{245, 245, 245, {}},
  Rgb24{246, 246, 246, {}},
  Rgb24{247, 247, 247, {}},
  Rgb24{248, 248, 248, {}},
  Rgb24{249, 249, 249, {}},
  Rgb24{250, 250, 250, {}},
  Rgb24{252, 252, 252, {}},
  Rgb24{253, 253, 253, {}},
  Rgb24{255, 255, 255, {}},
  Rgb24{0, 0, 0, {}},
  Rgb24{8, 0, 0, {}},
  Rgb24{16, 0, 0, {}},
  Rgb24{24, 0, 0, {}},
  Rgb24{32, 0, 0, {}},
  Rgb24{40, 0, 0, {}},
  Rgb24{48, 0, 0, {}},
  Rgb24{56, 0, 0, {}},
  Rgb24{64, 0, 0, {}},
  Rgb24{72, 0, 0, {}},
  Rgb24{80, 0, 0, {}},
  Rgb24{88, 0, 0, {}},
  Rgb24{96, 0, 0, {}},
  Rgb24{104, 0, 0, {}},
  Rgb24{112, 0, 0, {}},
  Rgb24{120, 0, 0, {}},
  Rgb24{128, 0, 0, {}},
  Rgb24{135, 0, 0, {}},
  Rgb24{143, 0, 0, {}},
  Rgb24{151, 0, 0, {}},
  Rgb24{159, 0, 0, {}},
  Rgb24{167, 0, 0, {}},
  Rgb24{175, 0, 0, {}},
  Rgb24{183, 0, 0, {}},
  Rgb24{191, 0, 0, {}},
  Rgb24{199, 0, 0, {}},
  Rgb24{207, 0, 0, {}},
  Rgb24{215, 0, 0, {}},
  Rgb24{223, 0, 0, {}},
  Rgb24{231, 0, 0, {}},
  Rgb24{239, 0, 0, {}},
  Rgb24{247, 0, 0, {}},
  Rgb24{255, 0, 0, {}},
  Rgb24{255, 255, 255, {}}
}; // pal8_default_table

Rgb24 to_palette_rgb24_default(const Pal8 x) { return pal8_default_table[x.val]; }