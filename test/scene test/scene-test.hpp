#pragma once
#include "host/main-base.hpp"
#include "util/macro.hpp"

class Scene_mgr;

class Scene_test: public Main_base {
public:
  Scene_test() = default;
  ~Scene_test() = default;
  void init(int argc, char *argv[]) override;
  void update(const Delta_time dt) override;
  void draw() const override;
}; // Scene_test
