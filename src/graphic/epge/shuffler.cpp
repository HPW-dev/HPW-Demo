#include <omp.h>
#include "shuffler.hpp"
#include "game/core/core.hpp"
#include "game/util/locale.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/graphic-util.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/util/rotation.hpp"
#include "util/math/limit.hpp"

namespace epge {
  
struct Shuffler::Impl {
  int _seed {97'997}; // ALMONDS
  int _block_sz {55};
  int _rnd_style {2};
  bool _rotate_blocks {true};
  bool _shuffle_blocks {true};
  bool _randomize_blocks {true};
  bool _unsafe {false};
  double _randomize_speed {0.12};
  mutable Image _buffer {};

  inline Str name() const { return "shuffler"; }
  #define LOCSTR(NAME) get_locale_str("epge.effect.shuffler." NAME)
  inline utf32 localized_name() const { return LOCSTR("name"); }
  inline utf32 desc() const { return LOCSTR("desc"); }

  inline void draw(Image& dst) const {
    assert(dst);
    check();
    _buffer.init(dst);

    // сколько получится кусочков:
    cauto block_mx = std::max<int>(1, dst.X / _block_sz + 1);
    cauto block_my = std::max<int>(1, dst.Y / _block_sz + 1);
    cut_blocks_and_shuffle(dst, _buffer, block_mx, block_my);
  }

  inline epge::Params params() {
    return epge::Params {
      new_shared<epge::Param_int>("seed", "value of the random generator that shuffles image blocks", _seed, 0, 0x7FFF'FFFF, 1, 10),
      new_shared<epge::Param_int>("block size", "size of cut image blocks", _block_sz, 2, 512, 1, 4),
      new_shared<epge::Param_bool>("blocks rotation", "apply image block rotation", _rotate_blocks),
      new_shared<epge::Param_bool>("blocks shuffling", "apply image block shuffling", _shuffle_blocks),
      new_shared<epge::Param_bool>("randomize blocks", "apply randomization of image blocks", _randomize_blocks),
      new_shared<epge::Param_double>("randomization speed", "speed of applying image block randomization", _randomize_speed, 0, 100, 0.01, 0.03),
      new_shared<epge::Param_int>("randomization style", "mode of randomization algorithm", _rnd_style, 1, 3, 1, 1),
      new_shared<epge::Param_bool>("unsafe", "but fast)", _unsafe),
    };
  }

  inline void check() const {
    assert(_block_sz > 1 && _block_sz <= 16'000);
    assert(_rnd_style > 0);
  }

  // нарезать картинку и размешать
  inline void cut_blocks_and_shuffle(Image& dst, cr<Image> src, const int mx, const int my) const {
    const int seed = _seed + (_randomize_blocks ? hpw::global_ticks * _randomize_speed : 0);
    const uint pos_mask = _shuffle_blocks ? num_max<uint>() : 0;
    const uint rot_by_90_deg_mask = _rotate_blocks ? num_max<uint>() : 0;

    #pragma omp parallel for simd collapse(2) if(_unsafe)
    cfor (y, my)
    cfor (x, mx) {
      const auto block = cut_block(src, x, y, _block_sz);
      cauto rnd = rnd_xy(seed, x, y, _rnd_style);
      // сколько раз провернуть картинку на 90 градусов
      const int rot_by_90_deg = (rnd % 4) & rot_by_90_deg_mask;
      const int pos_xor = rnd & pos_mask;
      insert_block(dst, block, (x ^ pos_xor) % mx, (y ^ pos_xor) % my, _block_sz, rot_by_90_deg);
    }
  }

  inline Image cut_block(cr<Image> src, const int x, const int y, const int block_sz) const {
    return cut(src, Recti(x * block_sz, y * block_sz, block_sz, block_sz), Image_get::MIRROR);
  }

  inline void insert_block(Image& dst, cr<Image> block, const int x, const int y, const int block_sz,
  const int rot_by_90_deg) const {
    insert(dst, rotate_90(block, rot_by_90_deg), Vec(x * block_sz, y * block_sz));
  }

  // получение случайного числа с помощью индексов и без сложного рандомного генератора
  inline static int rnd_xy(const int seed, const int x, const int y, const int style) {
    switch (style) {
      default:
      case 1: return (seed * ((x >> 4) ^ (y >> 5))) >> 7;
      case 2: return (seed ^ (x * y)) >> 5;
      case 3: return (seed * (x ^ y)) >> 7;
    }
    return 0;
  }
}; // Impl

EPGE_IMPL_MAKER(Shuffler)

} // epge ns
