#pragma once
#include "util/macro.hpp"
#include "util/str.hpp"

// базовый класс для ресурсов
class Resource {
  Str path {}; // путь к ресурсу
  bool generated {}; // был ли сгенерирован ресурс уже в игре

public:
  Resource() = default;
  inline Resource(CN<Str> _path, bool _generated=false) noexcept
    : path(_path), generated(_generated)
    {}
  inline CN<decltype(path)> get_path() const noexcept { return path; }
  inline CN<decltype(generated)> is_generated() const noexcept { return generated; }
  inline void set_path(CN<decltype(path)> val) noexcept { path = val; }
  inline void set_generated(CN<decltype(generated)> val) noexcept { generated = val; }
}; // Resource
