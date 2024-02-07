#pragma once
#include <string>
#include "vector-types.hpp"

using Cstr = const char*;
using Str = std::string;

constexpr auto str_npos {Str::npos};

using Strs = Vector<Str>;
