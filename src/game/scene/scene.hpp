#pragma once
#include "util/math/num-types.hpp"
#include "util/str.hpp"
#include "util/macro.hpp"
#include "util/mem-types.hpp"
#include "graphic/image/image-fwd.hpp"

// базовый класс для сцен (менюшки, катсцены, игра)
class Scene {
public:
  cautox NAME = "base scene";

  Scene() = default;
  virtual ~Scene() = default;
  virtual void update(const Delta_time dt);
  virtual void draw(Image& dst) const;
  virtual Str name() const = 0;
};

#define MAKE_SCENE_CLASS(CLASS_NAME) \
class CLASS_NAME final: public Scene { \
  struct Impl; \
  Unique<Impl> impl {}; \
public: \
  cautox NAME = #CLASS_NAME; \
  CLASS_NAME(); \
  ~CLASS_NAME(); \
  void update(const Delta_time dt) override; \
  void draw(Image& dst) const override; \
  inline Str name() const override { return NAME; } \
};

#define MAKE_SCENE_CLASS_IMPL(CLASS_NAME) \
CLASS_NAME::CLASS_NAME(): impl {new_unique<Impl>()} {} \
CLASS_NAME::~CLASS_NAME() {} \
void CLASS_NAME::update(const Delta_time dt) { impl->update(dt); } \
void CLASS_NAME::draw(Image& dst) const { impl->draw(dst); }
