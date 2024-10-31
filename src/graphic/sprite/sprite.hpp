#pragma once
#include <utility>
#include "game/util/resource.hpp"
#include "util/math/vec.hpp"
#include "graphic/image/image.hpp"

class Image;

class Sprite final: public Resource {
protected:
  Image m_image {};
  Image m_mask {}; // маска прозрачности. white - 100% alpha

public:
  inline int X() const noexcept { return m_image.X; }
  inline int Y() const noexcept { return m_image.Y; }
  inline int size() const noexcept { return m_image.size; }
  inline cr<Image> image() const noexcept { return m_image; }
  inline cr<Image> mask() const noexcept { return m_mask; }
  inline Image& image() noexcept { return m_image; }
  inline Image& mask() noexcept { return m_mask; }
  inline void set_image(cr<Image> image) noexcept { m_image = image; }
  inline void set_mask(cr<Image> mask) noexcept { m_mask = mask; }
  inline void move_image(Image&& image) noexcept { m_image = std::move(image); }
  inline void move_mask(Image&& mask) noexcept { m_mask = std::move(mask); }
  void init(cr<Sprite> other) noexcept;
  void init(int new_x, int new_y) noexcept;
  operator bool() const noexcept;
  Sprite() noexcept = default;
  Sprite(cr<Sprite> other) noexcept;
  Sprite(Sprite &&other) noexcept;
  Sprite(int new_x, int new_y) noexcept;
  Sprite& operator = (Sprite &&other) noexcept;
  Sprite& operator = (cr<Sprite> other) noexcept;
  ~Sprite() noexcept = default;
}; // Sprite
