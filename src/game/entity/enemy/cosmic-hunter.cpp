#include <cassert>
#include "cosmic-hunter.hpp"
#include "util/file/yaml.hpp"
#include "util/hpw-util.hpp"
#include "graphic/image/image.hpp"
#include "game/core/common.hpp"
#include "game/core/anims.hpp"
#include "game/core/entities.hpp"
#include "game/entity/player/player.hpp"
#include "game/entity/util/anim-ctx-util.hpp"
#include "game/entity/util/phys.hpp"
#include "game/entity/util/entity-util.hpp"
#include "util/math/random.hpp"
#include "util/math/mat.hpp"

Cosmic_hunter::Cosmic_hunter(): Proto_enemy(GET_SELF_TYPE) {}

void Cosmic_hunter::draw(Image& dst, const Vec offset) const {
  _info.external_part.draw(dst, *this, offset);
  Proto_enemy::draw(dst, offset);
}

void Cosmic_hunter::update(const Delta_time dt) {
  assert(hpw::shmup_mode); // вне шмап-мода этот класс не юзать
  // вращение внешней части TODO callback
  _info.external_part.set_default_deg(_info.external_deg);
  _info.external_deg = ring_deg(_info.external_deg + _info.initial_rot_spd * dt * (_info.external_rot_dir ? 1 : -1));

  cauto player = hpw::entity_mgr->get_player();
  return_if( !player);
  Proto_enemy::update(dt);
  
  // повернуться в игроку
  bool rot_to_right = need_rotate_right(*this, *player);
  phys.set_rot_spd(_info.rotate_speed);
  phys.set_invert_rotation( !rot_to_right);

  cfor (_, _info.shoot_timer.update(dt)) {
    auto bullet = hpw::entity_mgr->make(this, _info.bullet_name, phys.get_pos());
    bullet->phys.set_speed(_info.bullet_speed);
    bullet->status.ignore_scatter = true;

    real deg_to;
    switch (rndu(3)) {
      default:
      case 0: { // упреждение
        auto bullet_target = predict(bullet->phys, player->phys);
        deg_to = deg_to_target(bullet->phys.get_pos(), bullet_target);
        break;
      }
      case 1: { // напрямую
        deg_to = deg_to_target(bullet->phys.get_pos(), player->phys.get_pos());
        break;
      }
      case 2: { // напрямую с отклоениями
        deg_to = deg_to_target(bullet->phys.get_pos(), player->phys.get_pos());
        deg_to += rndr(-_info.shoot_deg, _info.shoot_deg);
        break;
      }
    } // switch shoot type

    bullet->phys.set_deg(deg_to);
  } // for timer
} // update

cp<Hitbox> Cosmic_hunter::get_hitbox() const {
  cauto proto_enemy_hitbox = Proto_enemy::get_hitbox();
  cauto external_hitbox = anim_ctx_util::get_hitbox(anim_ctx, _info.external_deg, *this);
  return proto_enemy_hitbox;
}

struct Cosmic_hunter::Loader::Impl {
  Info _info {};

  inline explicit Impl(cr<Yaml> config) {
    _info.shoot_timer = Timer( config.get_real("shoot_timer") );
    _info.speed = pps( config.get_real("speed") );
    _info.rotate_speed = pps( config.get_real("rotate_speed") );
    _info.bullet_speed = pps( config.get_real("bullet_speed") );
    _info.shoot_deg = config.get_real("shoot_deg");
    _info.bullet_name = config.get_str("bullet_name");

    cauto anim_node = config["animation"];
    assert(anim_node.check());
    cauto external_part_name = anim_node.get_str("foreground_name");
    cauto external_anim = hpw::anim_mgr->find_anim(external_part_name).get();
    _info.external_part.set_anim(external_anim);
    _info.initial_rot_spd = pps(anim_node.get_real("initial_rot_spd", 8.0));
    _info.external_rot_dir = rndb();
    
    assert(_info.bullet_speed > 0);
    assert(_info.rotate_speed > 0);
    assert(_info.speed > 0);
    assert(_info.initial_rot_spd > 0);
    assert(_info.initial_rot_spd < pps(360.0));
    assert( !_info.bullet_name.empty());
  } // c-tor

  inline Entity* operator()(Entity* master, const Vec pos, Entity* parent) {
    assert(parent);
    assert(parent->type == ENTITY_TYPE(Cosmic_hunter));
    auto it = ptr2ptr<Cosmic_hunter*>(parent);
    it->_info = _info;
    it->_info.shoot_timer.randomize_stable();
    it->phys.set_speed(_info.speed);
    // при спавне сразу смотрим в сторону игрока
    it->phys.set_deg( deg_to_target(it->phys.get_pos(), hpw::entity_mgr->target_for_enemy()) );
    return parent;
  } // op ()
}; // Impl

Cosmic_hunter::Loader::Loader(cr<Yaml> config)
: Proto_enemy::Loader(config)
, impl{new_unique<Impl>(config)}
{}

Entity* Cosmic_hunter::Loader::operator()(Entity* master, const Vec pos, Entity* parent) {
  auto ret = hpw::entity_mgr->allocate<Cosmic_hunter>();
  Proto_enemy::Loader::operator()(master, pos, ret);
  impl->operator()(master, pos, ret);
  ret->status.fixed_deg = true; // чтобы хитбокс не вращался
  return ret;
}

Cosmic_hunter::Loader::~Loader() {}
