#include <cassert>
#include "mirror.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/rotation.hpp"
#include "graphic/util/graphic-util.hpp"

namespace epge {

struct Mirror::Impl final {
  bool _mirror_v {true};
  bool _mirror_h {false};
  bool _rotate_180 {false};

  inline Str name() const noexcept { return "mirror"; }
  inline Str desc() const noexcept { return "mirrors the game frame"; }

  inline void draw(Image& dst) const noexcept {
    assert(dst);

    if (_mirror_v)
      insert_fast(dst, mirror_v(dst));

    if (_mirror_h)
      insert_fast(dst, mirror_h(dst));

    if (_rotate_180)
      insert_fast(dst, rotate_180(dst));
  }

  inline Params params() noexcept {
    return Params {
      new_shared<Param_bool>("vertical", "mirrors the game frame vertically", _mirror_v),
      new_shared<Param_bool>("horizontal", "mirrors the game frame horizontally", _mirror_h),
      new_shared<Param_bool>("rotate", "rotate the game frame by 180 degrees", _rotate_180),
    };
  }
}; // Impl

Mirror::Mirror(): impl{new_unique<Impl>()} {}
Mirror::~Mirror() {}
Str Mirror::name() const noexcept { return impl->name(); }
Str Mirror::desc() const noexcept { return impl->desc(); }
void Mirror::draw(Image& dst) const noexcept { impl->draw(dst); }
Params Mirror::params() noexcept { return impl->params(); }

} // epge ns
