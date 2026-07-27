#include <utility>
#include "layer-simple.hpp"

void Layer_simple::update(const Vec vel, Delta_time dt)
  { pos += vel * motion_ratio * dt; }

void Layer_simple::draw(Image& dst, int optional) const {
  tilemap.draw(pos, dst, bf, optional); 
}

Layer_simple::Layer_simple(cr<Str> tilemap_archive, const Vec _pos,
real _motion_ratio, blend_pf _bf)
: tilemap {tilemap_archive, true}
, pos {_pos}
, motion_ratio {_motion_ratio}
, bf {_bf}
{}

Layer_simple::Layer_simple(Layer_simple&& other)
: tilemap {std::move(other.tilemap)}
, pos {other.pos}
, motion_ratio {other.motion_ratio}
, bf {other.bf}
{}
