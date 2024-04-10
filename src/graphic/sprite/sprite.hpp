#pragma once
#include "game/util/resource.hpp"
#include "util/math/vec.hpp"
#include "util/mem-types.hpp"

class Image;

class Sprite final: public Resource {
protected:
  Shared<Image> _image {};
  Shared<Image> _mask {}; /// маска прозрачности. white - 100% alpha

public:
  int X() const;
  int Y() const;
  int size() const;
  inline Image* get_image() { return _image.get(); }
  inline Image* get_mask() { return _mask.get(); }
  inline CP<Image> get_image() const { return _image.get(); }
  inline CP<Image> get_mask() const { return _mask.get(); }
  void set_image(CN<Image> image) noexcept;
  void set_mask(CN<Image> mask) noexcept;
  void move_image(Image&& image) noexcept;
  void move_mask(Image&& mask) noexcept;
  void init(CN<Sprite> other) noexcept;
  void init(int new_x, int new_y) noexcept;
  operator bool() const;
  Sprite() = default;
  Sprite(CN<Sprite> other) noexcept;
  Sprite(Sprite &&other) noexcept;
  Sprite(int new_x, int new_y) noexcept;
  Sprite& operator = (Sprite &&other) noexcept;
  Sprite& operator = (CN<Sprite> other) noexcept;
  ~Sprite() = default;
}; // Sprite
