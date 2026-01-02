#pragma once
#include <utility>
#include "game/util/resource.hpp"
#include "util/math/vec.hpp"
#include "graphic/image/image.hpp"
#include "graphic/image/image-fwd.hpp"

class Sprite final: public Resource {
protected:
  Image _image {};
  Image _mask {}; // маска прозрачности. white - 100% alpha

public:
  int X() const { return _image.X; }
  int Y() const { return _image.Y; }
  int size() const { return _image.size; }
  cr<Image> image() const { return _image; }
  cr<Image> mask() const { return _mask; }
  Image& image() { return _image; }
  Image& mask() { return _mask; }
  void set_image(cr<Image> image) { _image = image; }
  void set_mask(cr<Image> mask) { _mask = mask; }
  void move_image(Image&& image) { _image = std::move(image); }
  void move_mask(Image&& mask) { _mask = std::move(mask); }
  void init(cr<Sprite> other);
  void init(int new_x, int new_y);
  operator bool() const;
  void set_path(cr<Str> val) noexcept override;
  Sprite() = default;
  Sprite(cr<Sprite> other);
  Sprite(Sprite &&other);
  Sprite(int new_x, int new_y);
  Sprite& operator = (Sprite &&other);
  Sprite& operator = (cr<Sprite> other);
  ~Sprite() = default;
}; // Sprite
