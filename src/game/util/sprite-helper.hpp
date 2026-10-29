#pragma once
#include "engine/graphic/image/image-fwd.hpp"
#include "util/str.hpp"

// Ищет картинку в банке спрайтов, если нету то взрыв
cr<Image> find_image_safe(cr<Str> name);
