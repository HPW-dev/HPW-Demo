#pragma once
#include "util/str.hpp"
#include "util/mempool.hpp"
#include "graphic/image/color.hpp"

class Yaml;
class Entity;
class Frame;

enum class Draw_frames_mode { sprite=0, mask, image };

namespace editor {

inline bool is_reset {false};
inline bool bg_color_red {true};
inline float bg_color_val {0.45f};
inline bool is_pause {false};
inline bool use_draw_cross {true};
inline bool use_zoom_x2 {false};
inline bool use_draw_frames {false};
inline Draw_frames_mode draw_frames_mode {Draw_frames_mode::sprite};
inline Pool_ptr(Entity) entity {};
inline Str anim_name {}; /// выбранная анимация из yml
inline bool other_bg {false}; /// показывать более сложный фон

} // editor ns
