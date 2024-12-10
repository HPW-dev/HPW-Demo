#include "shuffler.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/graphic-util.hpp"

namespace epge {

struct Shuffler::Impl final {
  inline Str name() const noexcept { return "shuffler"; }
  inline Str desc() const noexcept { return "breaks image into squares and shuffles them"; }

  inline void draw(Image& dst) const noexcept {
    assert(dst);
    // TODO
  }

  inline epge::Params params() noexcept {
    return epge::Params {}; // TODO
  }
}; // Impl

Shuffler::Shuffler(): impl{new_unique<Impl>()} {}
Shuffler::~Shuffler() {}
Str Shuffler::name() const noexcept { return impl->name(); }
Str Shuffler::desc() const noexcept { return impl->desc(); }
void Shuffler::draw(Image& dst) const noexcept { impl->draw(dst); }
Params Shuffler::params() noexcept { return impl->params(); }

} // epge ns
