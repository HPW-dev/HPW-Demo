#pragma once

using ldouble = long double;
using real = float;
using uint = unsigned;
using byte = unsigned char;
using i64_t = signed long long;
using Uid = unsigned short;

static_assert(sizeof(byte) == 1);
static_assert(sizeof(i64_t) == 8);
static_assert(sizeof(Uid) == 2);
static_assert(sizeof(uint) >= 4 && sizeof(uint) <= 8);
