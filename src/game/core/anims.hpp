#pragma once
#include "graphic/animation/anim-mgr.hpp"

namespace hpw {

inline Unique<Anim_mgr> anim_mgr {}; // управление анимациями

// если true, анимации будут грузиться по запросу, а не все сразу
inline bool lazy_load_anim {
#ifdef DEBUG
  true
#else
  false
#endif
};

} // hpw ns
