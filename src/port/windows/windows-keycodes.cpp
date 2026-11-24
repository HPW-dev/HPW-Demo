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
#else // Linux:
  sconst std::unordered_map<int, utf32> table = []() {
    std::unordered_map<int, utf32> map;
    auto add = [&](int glfw_key, utf32 name) {
      int sc = glfwGetKeyScancode(glfw_key);
      if (sc != -1)
        map[sc] = name;
    };

    add(GLFW_KEY_ESCAPE, U"ESC");
    add(GLFW_KEY_1, U"1");
    add(GLFW_KEY_2, U"2");
    add(GLFW_KEY_3, U"3");
    add(GLFW_KEY_4, U"4");
    add(GLFW_KEY_5, U"5");
    add(GLFW_KEY_6, U"6");
    add(GLFW_KEY_7, U"7");
    add(GLFW_KEY_8, U"8");
    add(GLFW_KEY_9, U"9");
    add(GLFW_KEY_0, U"0");
    add(GLFW_KEY_MINUS, U"-");
    add(GLFW_KEY_EQUAL, U"=");
    add(GLFW_KEY_BACKSPACE, U"bs");
    add(GLFW_KEY_TAB, U"Tab");
    add(GLFW_KEY_Q, U"Q");
    add(GLFW_KEY_W, U"W");
    add(GLFW_KEY_E, U"E");
    add(GLFW_KEY_R, U"R");
    add(GLFW_KEY_T, U"T");
    add(GLFW_KEY_Y, U"Y");
    add(GLFW_KEY_U, U"U");
    add(GLFW_KEY_I, U"I");
    add(GLFW_KEY_O, U"O");
    add(GLFW_KEY_P, U"P");
    add(GLFW_KEY_LEFT_BRACKET, U"[");
    add(GLFW_KEY_RIGHT_BRACKET, U"]");
    add(GLFW_KEY_ENTER, U"Enter");
    add(GLFW_KEY_LEFT_CONTROL, U"CTRL");
    add(GLFW_KEY_RIGHT_CONTROL, U"RCtrl");
    add(GLFW_KEY_A, U"A");
    add(GLFW_KEY_S, U"S");
    add(GLFW_KEY_D, U"D");
    add(GLFW_KEY_F, U"F");
    add(GLFW_KEY_G, U"G");
    add(GLFW_KEY_H, U"H");
    add(GLFW_KEY_J, U"J");
    add(GLFW_KEY_K, U"K");
    add(GLFW_KEY_L, U"L");
    add(GLFW_KEY_SEMICOLON, U";");
    add(GLFW_KEY_APOSTROPHE, U"'");
    add(GLFW_KEY_GRAVE_ACCENT, U"`");
    add(GLFW_KEY_LEFT_SHIFT, U"LShift");
    add(GLFW_KEY_BACKSLASH, U"\\");
    add(GLFW_KEY_Z, U"Z");
    add(GLFW_KEY_X, U"X");
    add(GLFW_KEY_C, U"C");
    add(GLFW_KEY_V, U"V");
    add(GLFW_KEY_B, U"B");
    add(GLFW_KEY_N, U"N");
    add(GLFW_KEY_M, U"M");
    add(GLFW_KEY_COMMA, U",");
    add(GLFW_KEY_PERIOD, U".");
    add(GLFW_KEY_SLASH, U"/");
    add(GLFW_KEY_RIGHT_SHIFT, U"RShift");
    add(GLFW_KEY_PRINT_SCREEN, U"PrtSc");
    add(GLFW_KEY_LEFT_ALT, U"Alt");
    add(GLFW_KEY_RIGHT_ALT, U"RAlt");
    add(GLFW_KEY_SPACE, U"Space");
    add(GLFW_KEY_CAPS_LOCK, U"Caps");
    add(GLFW_KEY_F1, U"F1");
    add(GLFW_KEY_F2, U"F2");
    add(GLFW_KEY_F3, U"F3");
    add(GLFW_KEY_F4, U"F4");
    add(GLFW_KEY_F5, U"F5");
    add(GLFW_KEY_F6, U"F6");
    add(GLFW_KEY_F7, U"F7");
    add(GLFW_KEY_F8, U"F8");
    add(GLFW_KEY_F9, U"F9");
    add(GLFW_KEY_F10, U"F10");
    add(GLFW_KEY_F11, U"F11");
    add(GLFW_KEY_F12, U"F12");
    add(GLFW_KEY_NUM_LOCK, U"Num");
    add(GLFW_KEY_SCROLL_LOCK, U"Scroll");
    add(GLFW_KEY_KP_7, U"Home (7)");
    add(GLFW_KEY_KP_8, U"↑ (8)");
    add(GLFW_KEY_KP_9, U"PgUp (9)");
    add(GLFW_KEY_KP_SUBTRACT, U"-");
    add(GLFW_KEY_KP_4, U"← (4)");
    add(GLFW_KEY_KP_5, U"Center (5)");
    add(GLFW_KEY_KP_6, U"→ (6)");
    add(GLFW_KEY_KP_ADD, U"+");
    add(GLFW_KEY_KP_1, U"End (1)");
    add(GLFW_KEY_KP_2, U"↓ (2)");
    add(GLFW_KEY_KP_3, U"PgDn (3)");
    add(GLFW_KEY_KP_0, U"Ins");
    add(GLFW_KEY_KP_DECIMAL, U"Del");
    add(GLFW_KEY_UP, U"↑");
    add(GLFW_KEY_DOWN, U"↓");
    add(GLFW_KEY_LEFT, U"←");
    add(GLFW_KEY_RIGHT, U"→");
    add(GLFW_KEY_HOME, U"Home");
    add(GLFW_KEY_END, U"End");
    add(GLFW_KEY_PAGE_UP, U"PgUp");
    add(GLFW_KEY_PAGE_DOWN, U"PgDn");
    add(GLFW_KEY_INSERT, U"Ins");
    add(GLFW_KEY_DELETE, U"Del");

    return map;
  }();

  if (auto it = table.find(keycode); it != table.end())
    return it->second;
  return {};
#endif
}
