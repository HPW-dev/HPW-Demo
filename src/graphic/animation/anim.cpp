#include <cassert>
#include <stdexcept>
#include <sstream>
#include <utility>
#include "anim.hpp"
#include "frame.hpp"
#include "util/error.hpp"
#include "util/math/mat.hpp"
#include "util/mempool.hpp"
#include "game/entity/util/hitbox.hpp"
#include "game/util/polygon-helper.hpp"
#include "game/core/entities.hpp"

class Anim::Hitbox_diections {
  using Diections = Vector<Hitbox>;
  Diections diections {}; // таблица хитбоксов-направлений

  // получить хитбокс с таблицы по углу направления
  inline std::size_t degree_to_idx(real degree=0) const {
    auto diections_sz = diections.size();
    if (diections_sz == 0)
      return 0;
    // сдвиг угла для точного выбора направления
    auto diection_deg = 360.0 / diections_sz;
    degree = ring_deg(degree + (diection_deg * 0.5));
    // вычислить индекс в таблице
    std::size_t idx = (degree * (1.0 / 360.0)) * diections_sz;
    return idx;
  }

public:
  ~Hitbox_diections() = default;
  Hitbox_diections(cr<Hitbox_diections> other) = delete;
  Hitbox_diections(Hitbox_diections&& other) = delete;
  Hitbox_diections* operator =(Hitbox_diections&& other) = delete;

  inline Hitbox_diections* operator =(cr<Hitbox_diections> other) {
    for (crauto other_direct: other.diections) {
      Hitbox this_direct;
      this_direct.simple = other_direct.simple;
      // скопировать все полигоны
      for (crauto other_poly: other_direct.polygons)
        this_direct.polygons.emplace_back(other_poly);
      diections.emplace_back(std::move(this_direct));
    }
    return this;
  }

  inline explicit Hitbox_diections(uint directions_count=0)
  : diections (directions_count)
  {}

  inline void set(real degree, cr<Hitbox> src)
    { get(degree) = src; }

  inline Hitbox& get(real degree=0)
    { return diections.at(degree_to_idx(degree)); }
  
  inline std::size_t max_diections() const { return diections.size(); }
}; // Hitbox_diections

Anim::Anim()
: hitbox_diections { new_shared<Hitbox_diections>() }
{}

void Anim::add_frame(cr<Shared<Frame>> frame) { frames.emplace_back(frame); }

void Anim::insert(std::size_t pos, cr<Shared<Frame>> frame) {
  if (frames.empty()) {
    add_frame(frame);
    return;
  }
  auto it = frames.begin();
  std::advance(it, pos);
  frames.insert(it, frame);
}

void Anim::remove_frame(std::size_t frame_num) { 
  return_if (frames.empty());
  frames.erase(frames.begin() + frame_num);
}

void Anim::set_name(cr<Str> new_name) {
  iferror(new_name.empty(), "name is empty");
  name = new_name;
}

void Anim::swap_frame(std::size_t a, std::size_t b) {
  return_if (a == b);
  return_if (a >= frames.size());
  return_if (b >= frames.size());
  std::swap(frames.at(a), frames.at(b));
}

cp<Frame> Anim::get_frame(std::size_t frame_num) const {
  if (frames.empty() || frame_num >= frames.size())
    return nullptr;
  return frames[frame_num].get();
}

Shared<Frame> Anim::get_frame_shared(std::size_t frame_num) const {
  if (frames.empty() || frame_num >= frames.size())
    return nullptr;
  return frames[frame_num];
}

cp<Hitbox> Anim::get_hitbox(real degree) const {
  if ( !source_hitbox)
    return {};
  if (hitbox_diections->max_diections() == 0)
    return {};
  return &hitbox_diections->get(degree);
}

void Anim::update_hitbox(cr<Pool_ptr(Hitbox)> _hitbox, uint _directions) {
  assert(_directions > 0 && _directions <= 360);
  source_hitbox = _hitbox;

  // создать все хитбоксы направлений
  init_shared(hitbox_diections, _directions);
  auto max_diections = hitbox_diections->max_diections();
  auto degree_step = 360.0 / max_diections;
  cfor (i, max_diections) {
    auto degree = i * degree_step;
    auto& diection = hitbox_diections->get(degree);
    diection = *source_hitbox; // берём копию с эталона
    diection.rotate(degree); // поворачиваем всё что можно
    diection.simple = cover_polygons(diection.polygons); // внешний покрывающий хитбокс
  }
} // update_hitbox

cr<decltype(Anim::source_hitbox)> Anim::get_hitbox_source() const {
  return source_hitbox;
}

uint Anim::get_gitbox_directions() const {
  return hitbox_diections->max_diections();
}

Anim* Anim::operator =(cr<Anim> other) {
  this->name = other.name;
  this->source_hitbox = hpw::entity_mgr->get_hitbox_pool().new_object<Hitbox>();
  if (other.source_hitbox)
    *this->source_hitbox = *other.source_hitbox;
  else
    this->source_hitbox = {};

  // скопировать все кадры
  this->frames = {};
  for (crauto other_frame: other.frames) {
    continue_if( !other_frame);

    auto this_frame = new_shared<Frame>();
    *this_frame = *other_frame;

    this->frames.emplace_back( std::move(this_frame) );
  }

  if (other.hitbox_diections)
    *this->hitbox_diections = *other.hitbox_diections;
  return this;
}
