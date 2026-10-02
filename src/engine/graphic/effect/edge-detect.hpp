#pragma once
#include "util/macro.hpp"
#include "util/num-types.hpp"
#include "engine/graphic/image/image-fwd.hpp"

[[nodiscard]] Image edge_detect(cr<Image> src, const real sensivity=0.5);
