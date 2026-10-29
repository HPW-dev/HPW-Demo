#pragma once
#include "game/scene/scene.hpp"


// окно смены фонов меню
class Scene_bgp_select final: public Scene {
public:
  staticx Cstr NAME = "BGP select";

  Scene_bgp_select();
  ~Scene_bgp_select();
  void update(const Delta_time dt) override;
  void draw(Image& dst) const override;
  inline Str name() const override { return NAME; }

private:
  struct Impl;
  Unique<Impl> _impl {};
};
