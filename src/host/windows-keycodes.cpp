#include <unordered_map>
#include "windows-keycodes.hpp"

std::optional<utf32> get_windows_keycode_name(int keycode) {
#ifdef WINDOWS
  static const std::unordered_map<int, utf32> table {
    {1, U"ESC"},
    {2, U"1"},
    {3, U"2"},
    {4, U"3"},
    {5, U"4"},
    {6, U"5"},
    {7, U"6"},
    {8, U"7"},
    {9, U"8"},
    {10, U"9"},
    {11, U"0"},
    {12, U"-"},
    {13, U"="},
    {14, U"bs"},
    {15, U"Tab"},
    {16, U"Q"},
    {17, U"W"},
    {18, U"E"},
    {19, U"R"},
    {20, U"T"},
    {21, U"Y"},
    {22, U"U"},
    {23, U"I"},
    {24, U"O"},
    {25, U"P"},
    {26, U"["},
    {27, U"]"},
    {28, U"Enter"},
    {29, U"CTRL"},
    {30, U"A"},
    {31, U"S"},
    {32, U"D"},
    {33, U"F"},
    {34, U"G"},
    {35, U"H"},
    {36, U"J"},
    {37, U"K"},
    {38, U"L"},
    {39, U";"},
    {40, U"'"},
    {41, U"`"},
    {42, U"LShift"},
    {43, U"\\"},
    {44, U"Z"},
    {45, U"X"},
    {46, U"C"},
    {47, U"V"},
    {48, U"B"},
    {49, U"N"},
    {50, U"M"},
    {51, U","},
    {52, U"."},
    {53, U"/"},
    {54, U"RShift"},
    {55, U"PrtSc"},
    {56, U"Alt"},
    {57, U"Space"},
    {58, U"Caps"},
    {59, U"F1"},
    {60, U"F2"},
    {61, U"F3"},
    {62, U"F4"},
    {63, U"F5"},
    {64, U"F6"},
    {65, U"F7"},
    {66, U"F8"},
    {67, U"F9"},
    {68, U"F10"},
    {69, U"Num"},
    {70, U"Scroll"},
    {71, U"Home (7)"},
    {72, U"↑ (8)"},
    {73, U"PgUp (9)"},
    {74, U"-"},
    {75, U"← (4)"},
    {76, U"Center (5)"},
    {77, U"→ (6)"},
    {78, U"+"},
    {79, U"End (1)"},
    {80, U"↓ (2)"},
    {81, U"PgDn (3)"},
    {82, U"Ins"},
    {83, U"Del"},
    {87, U"F11"},
    {328, U"↑"},
    {336, U"↓"},
    {331, U"←"},
    {333, U"→"},
  };
  try {
    return table.at(keycode);
  } catch (...) {}
  return {};
#else
  #pragma message("Linux scancodes not defined")
  /*
  static const std::unordered_map<int, utf32> table {
    {0x29, U"ESC"},
    {0x52, U"↑"},
    {0x51, U"↓"},
    {0x50, U"←"},
    {0x51, U"→"},
    {0x16, U"S"},
  };
  try {
    return table.at(keycode);
  } catch (...) {}
  return {};
  */
  return {}; // на линуксе другие сканкоды
#endif
} // get_windows_keycode_name
