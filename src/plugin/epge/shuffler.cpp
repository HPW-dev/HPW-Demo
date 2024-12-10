#include "shuffler.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/graphic-util.hpp"

namespace epge {

struct Shuffler::Impl final {
  int _seed {97'997}; // ALMONDS
  int _block_sz {8};
  bool _rotate_blocks {true};
  bool _shuffle_blocks {true};
  bool _randomize_blocks {false};
  double _randomize_speed {1.0};

  inline Str name() const noexcept { return "shuffler"; }
  inline Str desc() const noexcept { return "breaks image into squares and shuffles them"; }

  inline void draw(Image& dst) const noexcept {
    assert(dst);
    // TODO
  }

  inline epge::Params params() noexcept {
    return epge::Params {
      new_shared<epge::Param_int>("seed", "value of the random generator that shuffles image blocks", _seed, 0, 0x7FFF'FFFF, 1, 10),
      new_shared<epge::Param_int>("block size", "size of cut image blocks", _block_sz, 1, 512, 1, 4),
      new_shared<epge::Param_bool>("blocks rotation", "apply image block rotation", _rotate_blocks),
      new_shared<epge::Param_bool>("blocks shuffling", "apply image block shuffling", _shuffle_blocks),
      new_shared<epge::Param_bool>("randomize blocks", "apply randomization of image blocks", _randomize_blocks),
      new_shared<epge::Param_double>("randomization speed", "speed of applying image block randomization", _randomize_speed, 0, 100, 0.01, 0.05),
    };
  }
}; // Impl

Shuffler::Shuffler(): impl{new_unique<Impl>()} {}
Shuffler::~Shuffler() {}
Str Shuffler::name() const noexcept { return impl->name(); }
Str Shuffler::desc() const noexcept { return impl->desc(); }
void Shuffler::draw(Image& dst) const noexcept { impl->draw(dst); }
Params Shuffler::params() noexcept { return impl->params(); }

} // epge ns
