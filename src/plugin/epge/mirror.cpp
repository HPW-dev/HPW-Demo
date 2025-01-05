#include <cassert>
#include "mirror.hpp"
#include "graphic/image/image.hpp"

namespace epge {

struct Mirror::Impl final {

  inline Str name() const noexcept { return "mirror"; }
  inline Str desc() const noexcept { return "mirrors the game frame"; }

  inline void draw(Image& dst) const noexcept {
    assert(dst);
  }

  inline epge::Params params() noexcept {
    return epge::Params {
      
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
