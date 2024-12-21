#pragma once
#include <functional>
#include "util/mem-types.hpp"
#include "util/math/num-types.hpp"

class Image;

// применяет список эффектов к картинке
class Effect_mgr final {
public:
  // если эффект вернёт true, значит он завершился
  class Effect {
  public:
    Effect() = default;
    virtual ~Effect() = default;
    inline virtual bool update(const Delta_time dt) { return true; }
    inline virtual void draw(Image& dst) const {}
  };

  Effect_mgr();
  ~Effect_mgr();
  void move_to_front(Shared<Effect>&& effect); // вставить эффект первым
  void move_to_back(Shared<Effect>&& effect); // вставить эффект последним
  void update(const Delta_time dt);
  void draw(Image& dst) const;

private:
  struct Impl;
  Unique<Impl> impl {};
};
