#pragma once
#include "util/macro.hpp"
#include "util/mem-types.hpp"
#include "util/mempool.hpp"
#include "util/math/num-types.hpp"
#include "util/vector-types.hpp"
#include "util/str.hpp"

struct Direct;
class Frame;
class Hitbox;
using Frames = Vector<Shared<Frame>>;

class Anim final {
  Frames frames {};
  Str name {};
  Pool_ptr(Hitbox) source_hitbox {}; // эталонный хитбокс, для генерации повёрнутых версий

  class Hitbox_diections;
  // pimpl-класс для храненния направлений хитбоксов
  Shared<Hitbox_diections> hitbox_diections {};

public:
  Anim();
  ~Anim() = default;
  Anim* operator =(cr<Anim> other);
  Anim(cr<Anim> other) = delete;
  Anim(Anim&& other) = delete;
  Anim* operator =(Anim&& other) = delete;

  void add_frame (cr<Shared<Frame>> frame);
  void insert(std::size_t pos, cr<Shared<Frame>> frame);
  void remove_frame (std::size_t frame_num);
  void swap_frame (std::size_t a, std::size_t b);
  void set_name (cr<Str> new_name);
  
  inline Anim (cr<Frames> in): frames(in) {}
  cp<Frame> get_frame (std::size_t frame_num) const;
  Shared<Frame> get_frame_shared(std::size_t frame_num) const;
  inline bool frame_empty() const { return frames.empty(); }
  inline auto frame_count() const { return frames.size(); }
  inline crauto get_frames() const { return frames; }
  inline cr<Str> get_name() const { return name; }

  // получить хитбокс анимации по иуглу
  cp<Hitbox> get_hitbox(real degree=0) const;
  /** назначить новых хитбокс
  @param _hitbox хитбокс для эталона
  @param _directions число разворотов хитбокса */
  void update_hitbox(cr<Pool_ptr(Hitbox)> _hitbox, uint _directions=64);
  uint get_gitbox_directions() const;
  cr<decltype(source_hitbox)> get_hitbox_source() const;
}; // Anim
