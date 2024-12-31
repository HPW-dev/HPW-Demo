#include <omp.h>
#include <cassert>
#include "pixelate.hpp"
#include "graphic/image/image.hpp"

namespace epge {

struct Pixelate::Impl final {
  enum class Mode {
    neighbor = 0,
    average,
    maximum,
    minimum,
    MAX,
  };

  int _mode {scast<int>(Mode::average)};

  inline Str name() const noexcept { return "pixelate"; }
  inline Str desc() const noexcept { return "increases pixel sizes"; }

  inline void draw(Image& dst) const noexcept {
    assert(dst);

    switch (scast<Mode>(_mode)) {
      default:
      case Mode::neighbor: neighbor_draw(dst); break;
      case Mode::average:  average_draw(dst); break;
      case Mode::maximum:  maximum_draw(dst); break;
      case Mode::minimum:  minimum_draw(dst); break;
    }
  }

  inline void neighbor_draw(Image& dst) const noexcept {

  }

  inline void average_draw(Image& dst) const noexcept {

  }

  inline void maximum_draw(Image& dst) const noexcept {

  }

  inline void minimum_draw(Image& dst) const noexcept {

  }

  inline epge::Params params() noexcept {
    return epge::Params {
      new_shared<epge::Param_int>("mode", "adjacent pixel blending mode:\n"
        "  0 - neighbor, 1 - average,\n"
        "  2 - maximum,  3 - minimum,\n", _mode, 0, scast<int>(Mode::MAX)-1, 1, 1),
    };
  }
}; // Impl

Pixelate::Pixelate(): impl{new_unique<Impl>()} {}
Pixelate::~Pixelate() {}
Str Pixelate::name() const noexcept { return impl->name(); }
Str Pixelate::desc() const noexcept { return impl->desc(); }
void Pixelate::draw(Image& dst) const noexcept { impl->draw(dst); }
Params Pixelate::params() noexcept { return impl->params(); }

} // epge ns
