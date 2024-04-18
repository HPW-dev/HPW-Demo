#pragma once
#include <cstddef>
#include "game/util/resource.hpp"
#include "util/file/file.hpp"

// данные аудио файла
struct Audio: public Resource {
  Bytes data {};
  bool encoded_by_opus {};
};

// контекст для управления воспроизведения аудио файла
using Audio_ctx = std::uintptr_t;
inline constexpr Audio_ctx BAD_AUDIO = 0;
