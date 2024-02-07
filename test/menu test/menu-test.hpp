#pragma once
#include "host/main-base.hpp"
#include "util/macro.hpp"

class Scene_mgr;

class Menu_test final: public Main_base {
public:
  ~Menu_test();
  void init(int argc, char *argv[]) override;
  void update(double dt) override;
  void draw() const override;
}; // Menu_test
