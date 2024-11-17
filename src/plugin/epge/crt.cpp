#include "crt.hpp"

struct Epge_crt::Impl final {
  double _delme_double {};
  int _delme_int {};
  bool _delme_bool {};

  inline Str name() const noexcept { return "CRT"; }
  inline Str desc() const noexcept { return "Simulation of pixeled CRT display"; }

  inline void draw(Image& dst) const noexcept {
    // TODO
  }

  inline void update(const Delta_time dt) noexcept {
    // TODO
  }

  inline epge::Params params() noexcept {
    return epge::Params {
      new_shared<epge::Param_int>("int", "test int value (-100..500)", _delme_int, -100, 500, 5, 10),
      new_shared<epge::Param_double>("double", "test double value (-1..1)", _delme_double, -1, 1, 0.001, 0.1),
      new_shared<epge::Param_bool>("double", "test bool value", _delme_bool),
    };
  }
}; // Impl

Epge_crt::Epge_crt(): impl{new_unique<Impl>()} {}
Epge_crt::~Epge_crt() {}
Str Epge_crt::name() const noexcept { return impl->name(); }
Str Epge_crt::desc() const noexcept { return impl->desc(); }
void Epge_crt::draw(Image& dst) const noexcept { impl->draw(dst); }
void Epge_crt::update(const Delta_time dt) noexcept { impl->update(dt); }
epge::Params Epge_crt::params() noexcept { return impl->params(); }
