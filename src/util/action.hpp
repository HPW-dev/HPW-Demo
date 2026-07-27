#pragma once
#include <functional>
#include <unordered_map>
#include "util/str.hpp"
#include "util/vector-types.hpp"

using Action = std::function<void ()>;
using Actions = Vector<Action>;
using Action_table = std::unordered_map<Str, Action>;
