#pragma once
#include "engine/graphic/image/image-fwd.hpp"
#include "util/str.hpp"

// По тексту генерит картнку с QR-кодом
[[nodiscard]] Image qr_gen(cr<Str> text);
