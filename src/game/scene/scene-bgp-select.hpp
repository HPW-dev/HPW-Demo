#pragma once
#include "util/mem-types.hpp"
#include "scene.hpp"

// окно смены фонов меню
class Scene_bgp_select final: public Scene {
public:
  Scene_bgp_select();
  ~Scene_bgp_select();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
  inline Str name() const override { return "BGP select"; }

private:
  struct Impl;
  Unique<Impl> _impl {};
};
