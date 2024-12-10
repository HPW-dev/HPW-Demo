#include <omp.h>
#include "shuffler.hpp"
#include "game/core/core.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/graphic-util.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/util/rotation.hpp"

namespace epge {

struct Shuffler::Impl final {
  int _seed {97'997}; // ALMONDS
  int _block_sz {24};
  int _rnd_style {};
  bool _rotate_blocks {true};
  bool _shuffle_blocks {true};
  bool _randomize_blocks {false};
  double _randomize_speed {0.09};
  mutable Image _buffer {};

  inline Str name() const noexcept { return "shuffler"; }
  inline Str desc() const noexcept { return "breaks image into squares and shuffles them"; }

  inline void draw(Image& dst) const noexcept {
    assert(dst);
    check();
    _buffer.init(dst);

    // сколько получится кусочков:
    cauto block_mx = std::max<int>(1, dst.X / _block_sz);
    cauto block_my = std::max<int>(1, dst.Y / _block_sz);
    cut_blocks_and_shuffle(dst, _buffer, block_mx, block_my);
  }

  inline epge::Params params() noexcept {
    return epge::Params {
      new_shared<epge::Param_int>("seed", "value of the random generator that shuffles image blocks", _seed, 0, 0x7FFF'FFFF, 1, 10),
      new_shared<epge::Param_int>("block size", "size of cut image blocks", _block_sz, 2, 512, 1, 4),
      new_shared<epge::Param_bool>("blocks rotation", "apply image block rotation", _rotate_blocks),
      new_shared<epge::Param_bool>("blocks shuffling", "apply image block shuffling", _shuffle_blocks),
      new_shared<epge::Param_bool>("randomize blocks", "apply randomization of image blocks", _randomize_blocks),
      new_shared<epge::Param_double>("randomization speed", "speed of applying image block randomization", _randomize_speed, 0, 100, 0.01, 0.03),
      new_shared<epge::Param_int>("randomization style", "mode of randomization algorithm", _rnd_style, 1, 2, 1, 1),
    };
  }

  inline void check() const {
    assert(_block_sz > 1 && _block_sz <= 16'000);
  }

  // нарезать картинку и размешать
  inline void cut_blocks_and_shuffle(Image& dst, cr<Image> src, const int mx, const int my) const noexcept {
    const int seed = _seed + (_randomize_blocks ? hpw::global_ticks * _randomize_speed : 0);

    #pragma omp parallel for simd collapse(2)
    cfor (y, my)
    cfor (x, mx) {
      const auto block = cut_block(src, x, y, _block_sz);
      cauto rnd = rnd_xy(seed, x, y, _rnd_style);
      // сколько раз провернуть картинку на 90 градусов
      const int rot_by_90_deg = rnd % 4;
      insert_block(dst, block, x, y, _block_sz, rot_by_90_deg);
    }
  }

  inline Image cut_block(cr<Image> src, const int x, const int y, const int block_sz) const noexcept {
    return cut(src, Rect(x * block_sz, y * block_sz, block_sz, block_sz), Image_get::NONE);
  }

  inline void insert_block(Image& dst, cr<Image> block, const int x, const int y, const int block_sz,
  const int rot_by_90_deg) const noexcept {
    insert(dst, rotate_90(block, rot_by_90_deg), Vec(x * block_sz, y * block_sz));
  }

  // получение случайного числа с помощью индексов и без сложного рандомного генератора
  inline static int rnd_xy(const int seed, const int x, const int y, const int style) noexcept {
    switch (style) {
      default:
      case 1: return ((seed * ((x >> 4) ^ (y >> 5))) >> 7);
      case 2: return ((seed * (x ^ y)) >> 7);
    }
    return 0;
  }
}; // Impl

Shuffler::Shuffler(): impl{new_unique<Impl>()} {}
Shuffler::~Shuffler() {}
Str Shuffler::name() const noexcept { return impl->name(); }
Str Shuffler::desc() const noexcept { return impl->desc(); }
void Shuffler::draw(Image& dst) const noexcept { impl->draw(dst); }
Params Shuffler::params() noexcept { return impl->params(); }

} // epge ns
