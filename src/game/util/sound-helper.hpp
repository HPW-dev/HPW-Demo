#pragma once
#include "util/math/vec.hpp"

struct Vec3;

void load_sounds(); // загрузить звуки в звуковую систему
Vec3 to_sound_vel(const Vec src);
Vec3 to_sound_pos(const Vec src);
