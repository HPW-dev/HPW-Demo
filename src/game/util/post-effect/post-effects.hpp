#pragma once
#include <functional>
#include "util/mem-types.hpp"

class Image;

/// применяет список эффектов к картинке
class Effect_mgr final {
  struct Impl;
  Unique<Impl> impl {};

public:
  /// если эффект вернёт true, значит он завершился
  class Effect {
  public:
    Effect() = default;
    ~Effect() = default;
    inline virtual bool update(double dt) { return true; }
    inline virtual void draw(Image& dst) const {}
  };

  Effect_mgr();
  ~Effect_mgr();
  void move_to_front(Shared<Effect>&& effect); // вставить эффект первым
  void move_to_back(Shared<Effect>&& effect); // вставить эффект последним
  void update(double dt);
  void draw(Image& dst) const;
};
