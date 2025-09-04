#include <unordered_map>
#include <algorithm>
#include "vrtjoy.hpp"
#include "util/math/mat.hpp"
#include "util/math/vec-util.hpp"

struct Key_state {
  bool pressed {};
};

struct Virtual_joystick::Impl {
  Vec _dir {};
  real _degree {};
  real _intense {};
  mutable std::unordered_map<Virtual_joystick::Key, Key_state> _keys;

  inline void set_angle(real degree, real intense) {
    _degree = ring_deg(degree);
    intense = std::clamp<real>(intense, 0, 1);
    _intense = intense;
    _dir = deg_to_vec(degree) * intense; 
  }

  inline Vec direction() const { return _dir; }
  inline real get_degree() const { return _degree; }
  inline real get_intense() const { return _intense; }
  inline void press(Key key) { _keys[key].pressed = true; }
  inline void release(Key key) { _keys[key].pressed = false; }
  inline bool pressed(Key key) const { return _keys[key].pressed; }
  inline bool released(Key key) const { return !_keys[key].pressed; }
}; // Impl

Virtual_joystick::Virtual_joystick(): _impl {new_unique<Virtual_joystick::Impl>()} {}
Virtual_joystick::~Virtual_joystick() {}
void Virtual_joystick::set_angle(real degree, real intense) { _impl->set_angle(degree, intense); }
Vec Virtual_joystick::direction() const { return _impl->direction(); }
void Virtual_joystick::press(Key key) { _impl->press(key); }
void Virtual_joystick::release(Key key) { _impl->release(key); }
bool Virtual_joystick::pressed(Key key) const { return _impl->pressed(key); }
bool Virtual_joystick::released(Key key) const { return _impl->released(key); }
real Virtual_joystick::get_degree() const { return _impl->get_degree(); }
real Virtual_joystick::get_intense() const { return _impl->get_intense(); }
