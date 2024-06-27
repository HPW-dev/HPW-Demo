#include <cassert>
#include <unordered_map>
#include "entity-manager.hpp"
#include "entity-type.hpp"
#include "particle-loader.hpp"
#include "explosion-loader.hpp"
#include "bonus-loader.hpp"
#include "bullet-loader.hpp"
#include "entity.hpp"
#include "player/player-dark.hpp"
#include "collider/collider-empty.hpp"
#include "util/hitbox.hpp"
#include "util/phys.hpp"
#include "util/safecall.hpp"
#include "util/error.hpp"
#include "util/log.hpp"
#include "util/file/yaml.hpp"
#include "game/util/game-archive.hpp"
#include "game/util/game-util.hpp"
#include "game/core/time-scale.hpp"
#include "game/core/core.hpp"
#include "game/core/canvas.hpp"
#include "game/core/common.hpp"
#include "game/entity/util/scatter.hpp"
#include "game/entity/util/entity-util.hpp"
#include "game/entity/enemy/cosmic-hunter.hpp"
#include "game/entity/enemy/cosmic-waiter.hpp"
#include "game/entity/enemy/cosmic.hpp"
#include "game/entity/enemy/illaen.hpp"
#include "game/entity/enemy/snake.hpp"
#include "game/entity/enemy/enemy-tutorial.hpp"
#include "game/entity/player/ability/invise.hpp"

struct Entity_mgr::Impl {
  // за пределами этого расстояние пули за экраном умирают в шмап-моде
  constx real shmup_bound_for_bullet = 34;
  // за пределами этого расстояние объект за экраном умирает в шмап-моде
  constx real shmup_bound = 250;

  Entitys entities {}; // массив всех игровых объектов
  Shared<Collider> collision_resolver {}; // обработчик столкновений
  Mem_pool entity_pool {}; // мем-пул под объекты
  Mem_pool phys_pool {}; // мем-пул под физические контексты
  Mem_pool hitbox_pool {}; // мем-пул под хитбоксы
  // БД инициализаторов объектов
  std::unordered_map<Str, Shared<Entity_loader>> entity_loaders {};
  Vector<Scatter> scatters {}; // источники взрывных волн
  Entitys registrate_list {};
  // текущая ссылка на игрока, чтобы враги могли брать его в таргет
  Player* m_player {};
  bool m_visible {true}; // видимость игровых объектов

  inline Impl() {
    #ifndef ECOMEM
    entities.reserve(4'000);
    #endif
  }

  ~Impl() = default;

  inline CN<Entitys> get_entities() const { return entities; }

  inline void set_collider(CN<Shared<Collider>> new_collider)
    { collision_resolver = new_collider; }

  inline void draw(Image& dst, const Vec offset) const {
    return_if(!m_visible);
    // нарисовать нижний слой
    for (cnauto entity: entities)
      if (entity->status.live && !entity->status.layer_up)
        entity->draw(dst, offset);
    // нарисовать верхний слой
    for (cnauto entity: entities)
      if (entity->status.live && entity->status.layer_up)
        entity->draw(dst, offset);
  }

  inline void update(const Delta_time dt) {
    accept_registrate_list();
    update_scatters();
    update_entitys(dt);
    if (collision_resolver)
      (*collision_resolver)(entities, dt);
    bound_check();
    update_kills();
  } // update

  // применить список на добавление объектов из очереди registrate_list
  inline void accept_registrate_list() {
    entities.reserve(entities.size() + registrate_list.size());
    for (nauto elem: registrate_list)
      entities.emplace_back(std::move(elem));
    registrate_list.clear();
  }

  // применить все взрывные волны к объектам
  inline void update_scatters() {
    if (!scatters.empty()) {
      for (nauto entity: entities)
        if (entity->status.live && !entity->status.ignore_scatter) {
          for (cnauto scatter: scatters)
            scatter.accept(*entity);
        }
      scatters.clear();
    }
  }

  inline Entity* registrate(Entitys::value_type&& entity) {
    assert(entity);
    auto ret = registrate_list.emplace_back(std::move(entity)).get();
    return ret;
  }

  inline void clear() {
    detailed_log("Entity_mgr.clear\n");
    collision_resolver = {};
    m_player = {};
    entities.clear();
    entity_pool.release();
    phys_pool.release();
    hitbox_pool.release();
  }

  inline void update_entitys(const Delta_time dt) {
    assert(hpw::time_scale > 0);

    for (nauto entity: entities) {
      if (entity->status.live) {
        // изменять время для игрока и его объектов
        cauto player = get_player();
        if (entity.get() == player || entity->master == player)
          entity->update(dt / hpw::time_scale);
        else // все остальные сущности
          entity->update(dt);
      }
    } // if live
  } // update_entitys

  inline void update_kills() {
    for (nauto entity: entities) {
      cont_if( !entity);
      cont_if( !entity->status.live);

      // смерть от флага killed (нет жизней у объекта)
      if (entity->status.killed) {
        entity->kill();
        continue;
      }
      // смерть от конца анимации
      if (entity->status.kill_by_end_anim && entity->status.end_anim) {
        entity->kill();
        continue;
      }
    } // for entities

    // TODO чистка в ECOMEM
  } // update_kills

  inline void debug_draw(Image& dst) const
    { collision_resolver->debug_draw(dst, {}/* camera TODO */); }

  inline Mem_pool& get_phys_pool() { return phys_pool; }
  inline Mem_pool& get_hitbox_pool() { return hitbox_pool; }
  inline Mem_pool& get_entity_pool() { return entity_pool; }

  // по type определяет какой Entity_loader создать и передать ему параметры с конфига
  inline Shared<Entity_loader> make_entity_loader(CN<Str> type, CN<Yaml> config) {
    // регистрация загрузчиков объектов
    using Maker = std::function< Shared<Entity_loader> (CN<Yaml>) >;
    std::unordered_map<Str, Maker> table {
      {"explosion", [](CN<Yaml> config){ return new_shared<Explosion_loader>(config); } },
      {"bonus", [](CN<Yaml> config){ return new_shared<Bonus_loader>(config); } },
      {"bullet", [](CN<Yaml> config){ return new_shared<Bullet_loader>(config); } },
      {"particle", [](CN<Yaml> config){ return new_shared<Particle_loader>(config); } },
      {"enemy.illaen", [](CN<Yaml> config){ return new_shared<Illaen::Loader>(config); } },
      {"enemy.cosmic.hunter", [](CN<Yaml> config){ return new_shared<Cosmic_hunter::Loader>(config); } },
      {"enemy.cosmic.waiter", [](CN<Yaml> config){ return new_shared<Cosmic_waiter::Loader>(config); } },
      {"enemy.cosmic", [](CN<Yaml> config){ return new_shared<Cosmic::Loader>(config); } },
      {"enemy.tutorial", [](CN<Yaml> config){ return new_shared<Enemy_tutorial::Loader>(config); } },
      {"snake.head", [](CN<Yaml> config){ return new_shared<Enemy_snake_head::Loader>(config); } },
      {"snake.tail", [](CN<Yaml> config){ return new_shared<Enemy_snake_tail::Loader>(config); } },
      {"player.boo.dark", [](CN<Yaml> config){ return new_shared<Player_dark::Loader>(config); } },
    };

    try {
      return table.at(type)(config);
    } catch (CN<std::out_of_range> ex) {
      error("тип загрузчика объекта \"" << type << "\" не зарегистрирован\n" << ex.what());
    }

    return {};
  } // make_entity_loader

  inline void register_types() {
    entity_loaders.clear();

    #ifndef ECOMEM // при экономии памяти объекты подгружаются в момент вызова
      // загрузить все объекты из конфига
      auto config = load_entity_config();
      for (cnauto entity_name: config.root_tags()) {
        auto entity_node = config[entity_name];
        auto type = entity_node.get_str("type", "error type");
        entity_loaders[entity_name] = make_entity_loader(type, entity_node);
      }
    #endif
  } // register_types

  inline Entity* load_unknown_entity(Entity* master, CN<Str> name, const Vec pos) {
    // попытаться загрузить отсутствующий объект
    auto config = load_entity_config();
    auto entity_node = config[name];
    auto type = entity_node.get_str("type", "error type");
    entity_loaders[name] = make_entity_loader(type, entity_node);
        
    try {
      return ( *entity_loaders.at(name) )(master, pos);
    } catch (CN<std::out_of_range> ex) {
      error("нет инициализатора для \"" << name << "\"");
    }
    return {};
  }

  inline Entity* make(Entity* master, CN<Str> name, const Vec pos) {
    #ifdef ECOMEM
      return load_unknown_entity(master, name, pos);
    #endif

    iferror(entity_loaders.empty(), "вызови register_types для заполнения entity_loaders");

    try {
      auto entity_loader = entity_loaders.at(name);
      assert(entity_loader);
      auto ret = (*entity_loader)(master, pos);
      if (ret)
        ret->set_name(name);
      return ret;
    } catch (CN<std::out_of_range> ex) {
      #ifdef ECOMEM
        return load_unknown_entity(master, name, pos);
      #else
        error("нет инициализатора для \"" << name << "\"");
      #endif
    }
    return {};
  } // make

  inline Yaml load_entity_config() const {
    #ifdef EDITOR
      return Yaml(hpw::cur_dir + "config/entities.yml");
    #else
      return Yaml(hpw::archive->get_file("config/entities.yml"));
    #endif
  }

  inline void add_scatter(CN<Scatter> scatter) {
    return_if(scatter.power <= 0);
    return_if(scatter.range <= 0);
    scatters.emplace_back(scatter);
  }

  // удаление объектов за экраном
  inline void bound_check() {
    for (nauto entity: entities) {
      if (entity->status.live && !entity->status.ignore_bound) {
        cnauto entity_pos = entity->phys.get_pos();
        auto bound = entity->status.is_bullet ? shmup_bound_for_bullet : shmup_bound;

        // тихо убить объект, если он вышел за пределы
        if (
          entity_pos.x <= -bound ||
          entity_pos.x >= graphic::width + bound ||
          entity_pos.y <= -bound ||
          entity_pos.y >= graphic::height + bound
        ) {
          entity->status.live = false;
          entity->status.killed = true;
          entity->accept_kill_callbacks();
        }
      } // if live
    } // for entities
  } // bound_check

  // возвращает первый попавшийся мёртвый объект нужного типа
  inline Entity* find_avaliable_entity(const Entity_type type) {
    for (nauto entity: get_entities())
      if (entity->type == type && !entity->status.live)
        return entity.get();
    return {};
  }

  inline Player* get_player() const { return m_player; }
  inline void set_player(Player* player) { m_player = player; }

  Vec target_for_enemy() const {
    // если игрок есть
    if (cauto player = get_player(); player) {
      // если игрок не невидимый
      if (!player->find_ability(Ability_invise(*player).type_id())) {
        return player->phys.get_pos();
      }
    }
    // если игрока не нашли, стрелять куда попало
    return get_rand_pos_safe(0, 0, graphic::width, graphic::height);
  }

  inline void set_visible(const bool mode) { m_visible = mode; }
}; // Impl

Entity_mgr::Entity_mgr(): impl {new_unique<Impl>()} {}
Entity_mgr::~Entity_mgr() { impl->clear(); }
void Entity_mgr::draw(Image& dst, const Vec offset) const { impl->draw(dst, offset); }
void Entity_mgr::update(const Delta_time dt) { impl->update(dt); }
void Entity_mgr::clear() { impl->clear(); }
void Entity_mgr::set_collider(CN<Shared<Collider>> new_collider) { impl->set_collider(new_collider); }
void Entity_mgr::register_types() { impl->register_types(); }
Entity* Entity_mgr::make(Entity* master, CN<Str> name, const Vec pos) { return impl->make(master, name, pos); }
Entity* Entity_mgr::registrate(Entitys::value_type&& entity) { return impl->registrate(std::move(entity)); }
void Entity_mgr::add_scatter(CN<Scatter> scatter) { return impl->add_scatter(scatter); }
void Entity_mgr::debug_draw(Image& dst) const { impl->debug_draw(dst); }
Mem_pool& Entity_mgr::get_phys_pool() { return impl->get_phys_pool(); }
Mem_pool& Entity_mgr::get_hitbox_pool() { return impl->get_hitbox_pool(); }
Mem_pool& Entity_mgr::get_entity_pool() { return impl->get_entity_pool(); }
CN<Entitys> Entity_mgr::get_entities() const { return impl->get_entities(); }
Entity* Entity_mgr::find_avaliable_entity(const Entity_type type) { return impl->find_avaliable_entity(type); }
Player* Entity_mgr::get_player() const { return impl->get_player(); }
void Entity_mgr::set_player(Player* player) { impl->set_player(player); }
Vec Entity_mgr::target_for_enemy() const { return impl->target_for_enemy(); }
void Entity_mgr::set_visible(const bool mode) { return impl->set_visible(mode); }
