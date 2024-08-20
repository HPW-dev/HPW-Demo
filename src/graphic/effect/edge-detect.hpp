#pragma once
#include "util/macro.hpp"
#include "util/math/num-types.hpp"

class Image;

[[nodiscard]] Image edge_detect(cr<Image> src, const real sensivity=0.5);
