#include <cassert>
#include <stdexcept>
#include <utility>
#include "frame.hpp"
#include "util/error.hpp"
#include "util/log.hpp"
#include "graphic/util/graphic-util.hpp"
#include "graphic/util/rotation.hpp"
#include "graphic/util/rotsprite.hpp"
#include "graphic/sprite/sprite.hpp"
#include "util/math/mat.hpp"
#include "util/math/vec.hpp"
#include "util/math/vec-util.hpp"
#include "util/str-util.hpp"
#include "game/util/store.hpp"
#include "game/core/sprites.hpp"

Frame::Frame(): name {generate_frame_name()} {}

void Frame::init_directions(Source_ctx&& new_ctx) {
  source_ctx = std::move(new_ctx);
  reinit_directions_by_source();
}

void Frame::clear_directions() {
  directions.clear();
  degree_frag = {};
}

void Frame::reinit_directions_by_source() {
  clear_directions();
  return_if (source_ctx.max_directions == 0);
  if (source_ctx.direct_0.sprite.expired()) {
    detailed_log("ctx sprite is empty\n");
    return;
  }
  assert(source_ctx.max_directions <= 360);
  
  degree_frag = 360.0 / source_ctx.max_directions;

  // без разворотов
  if (source_ctx.max_directions == 1) {
    init_once();
    return;
  }

  // для генерации симметричных углов, можно копировать только одну четверть
  if (source_ctx.max_directions % 4 == 0) {
    init_quarters();
    return;
  }

  // генерировать поворотные кадры max_directions раз
  cfor (i, source_ctx.max_directions) {
    Direct direct;
    direct.offset = source_ctx.direct_0.offset;
    cauto new_path = source_ctx.direct_0.sprite.lock()->get_path() + "." + n2s(i);
    auto new_sprite = new_shared<Sprite>(
      rotate_and_optimize(*source_ctx.direct_0.sprite.lock(), degree_frag * i,
      direct.offset, source_ctx.rotate_offset, source_ctx.cgp, source_ctx.ccf)
    );
    new_sprite->set_generated(true);
    direct.sprite = hpw::sprites.move(new_path, std::move(new_sprite));
    directions.emplace_back(direct);
  }
} // init_directions

void Frame::init_once() {
  directions.emplace_back(
    source_ctx.direct_0.sprite,
    source_ctx.direct_0.offset
  );
}

void Frame::init_quarters() {
  // degree_frag уже задан в init_directions
  // сгенерировать только одну четверть поворотов
  auto max_quarter = source_ctx.max_directions / 4;
  // I
  cfor (i, max_quarter) {
    Direct direct;
    direct.offset = source_ctx.direct_0.offset;
    auto new_sprite = new_shared<Sprite>(
      rotate_and_optimize(*source_ctx.direct_0.sprite.lock(), degree_frag * i,
        direct.offset, source_ctx.rotate_offset, source_ctx.cgp, source_ctx.ccf)
    );
    new_sprite->set_generated(true);
    cauto new_path = source_ctx.direct_0.sprite.lock()->get_path() + "." + n2s(i);
    direct.sprite = hpw::sprites.move(new_path, std::move(new_sprite));
    directions.emplace_back( std::move(direct) );
  }
  // оставшиеся четверти дополнить
  cfor (quarter, 3) {
    cfor (i, max_quarter) {
      Direct direct;
      // берётся разворот с предыдущей четверти
      cauto idx = i + max_quarter * quarter;
      cauto src_direction = directions.at(idx);

      if (!src_direction.sprite.expired()) {
        auto new_sprite = new_shared<Sprite>(rotate_90(*src_direction.sprite.lock()));
        new_sprite->set_generated(true);
        cauto new_path = src_direction.sprite.lock()->get_path()
          + "." + n2s(max_quarter + quarter * i);
        direct.sprite = hpw::sprites.move(new_path, std::move(new_sprite));
        direct.offset = rotate_deg({}, src_direction.offset, 90);
        // коррекция повёрнутого оффсета
        direct.offset.x -= direct.sprite.lock()->X() - 1;
      }
      directions.emplace_back( std::move(direct) );
    }
  }
}  // init_quarters

cp<Direct> Frame::get_direct(real degree) const {
  accept_degree_offset(degree);
  iferror (degree < 0 || degree > 360.0, "degree not clamped");
  return_if (directions.empty(), nullptr);
  
  // перекрутка, чтобы было удобно выбирать индекс по углу
  degree += degree_frag * real(0.5); // отсчёт с половинки деления
  degree = ring_deg(degree);
  int i_deg = degree / degree_frag;

  iferror(std::size_t(i_deg) >= directions.size(),
    "get_from_xfrag: i_deg >= _directs.size()");
    
  auto& ret = directions[i_deg];
  if (!ret.sprite.expired())
    return &ret;
  return nullptr;
} // get_direct

Vector<Vec> Frame::get_offsets() const {
  if (directions.empty())
    return {};
  Vector<Vec> offsets;
  for (crauto direct: directions)
    offsets.emplace_back(direct.offset);
  return offsets;
}

void Frame::accept_degree_offset(real &degree) const {
  // применить смещение угла
  if (degree_offset != 0) {
    degree += degree_offset;
    degree = ring_deg(degree);
  }
}

Frame::Frame(cr<Frame> other)
: directions { other.directions }
, degree_frag { other.degree_frag }
, name { generate_frame_name() }
, source_ctx { other.source_ctx }
, duration { other.duration }
, degree_offset { other.degree_offset }
{}

Frame& Frame::operator = (cr<Frame> other) {
  if (this == std::addressof(other))
    return *this;

  // копиря картинок с разворотами
  this->directions.clear();
  for (crauto direct: other.directions)
    this->directions.emplace_back(direct);
  degree_frag = other.degree_frag;
  name = other.name;
  source_ctx = other.source_ctx;
  duration = other.duration;
  degree_offset = other.degree_offset;
  return *this;
}

Frame::Frame(Frame&& other) noexcept
: directions {std::move( other.directions )}
, degree_frag {std::move( other.degree_frag )}
, name {std::move( other.name )}
, source_ctx {std::move( other.source_ctx )}
, duration {other.duration}
, degree_offset {other.degree_offset}
{ 
  other.degree_frag = {};
  other.source_ctx = {};
  other.duration = {};
  other.degree_offset = {};
}

Frame& Frame::operator = (Frame&& other) noexcept {
  if (this == std::addressof(other))
    return *this;

  directions = std::move(other.directions);
  degree_frag = other.degree_frag;
  name = std::move(other.name);
  source_ctx = other.source_ctx;
  duration = other.duration;
  degree_offset = std::move(other.degree_offset);

  other.source_ctx = {};
  other.duration = 0;
  other.degree_offset = {};
  return *this;
}

Str Frame::generate_frame_name() {
  static uint frame_uid = 0;
  return "Frame_UID_" + n2s(frame_uid++);
}

cr<Str> Frame::get_name() const {
  assert(!name.empty()); // if empty - need generate_frame_name()
  return name;
}
