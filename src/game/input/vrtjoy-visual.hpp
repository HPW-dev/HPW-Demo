#pragma once
#include "util/macro.hpp"
#include "util/math/vec.hpp"
#include "graphic/image/image-fwd.hpp"

class Virtual_joystick;

void draw(Image& dst, cr<Virtual_joystick> vrtjoy, const Veci pos);
