#include <cassert>
#include <algorithm>
#include "camera.hpp"
#include "util/math/vec.hpp"
#include "util/math/vec-util.hpp"
#include "game/core/canvas.hpp"
#include "game/core/common.hpp"
#include "game/core/entities.hpp"
#include "game/entity/player.hpp"
#include "game/entity/util/phys.hpp"

struct Camera::Impl {
  Vec screen_center {};
  constx Delta_time max_shake_time = 0.37;
  constx real shake_amplify = 1.0 / 3.0;
  constx real shake_len = 9; // на сколько сильное смещение при тряске
  Delta_time m_shake_time {}; // пока не кончится, камера будет трястись

  inline Impl()
  : screen_center(graphic::width / 2.0, graphic::height / 2.0)
  {}

  inline Vec get_offset() const {
    Vec ret {};
    auto player = hpw::entity_mgr->get_player();
    if ( !player)
      return ret;

    if ( !hpw::shmup_mode) // неподвижная камера для шмап-мода в {0, 0}
      ret = screen_center - player->phys.get_pos(); // следовать за игроком
    // добавить тряску
    ret += get_shake_offset();
    return ret;
  }

  inline Vec get_shake_offset() const {
    cauto shake_power = shake_len * (m_shake_time / max_shake_time);
    return rand_normalized_graphic() * shake_power;
  }

  inline void add_shake(real intense) {
    m_shake_time = std::clamp<real>(
      m_shake_time + intense * shake_amplify, 0, max_shake_time);
  }

  inline void update(const Delta_time dt) {
    m_shake_time = std::max<Delta_time>(0, m_shake_time - dt);
  }
}; // Impl

Camera::Camera(): impl {new_unique<Impl>()} {}
Camera::~Camera() {}
Vec Camera::get_offset() const { return impl->get_offset(); }
void Camera::add_shake(real intense) { return impl->add_shake(intense); }
void Camera::update(const Delta_time dt) { return impl->update(dt); }
