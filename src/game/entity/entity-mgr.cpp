#include <cassert>
#include <unordered_map>
#include "entity-mgr.hpp"
#include "entity-type.hpp"
#include "particle-loader.hpp"
#include "explosion-loader.hpp"
#include "bonus-loader.hpp"
#include "bullet-loader.hpp"
#include "entity.hpp"
#include "host/command.hpp"
#include "player/player-dark.hpp"
#include "collider/collider-empty.hpp"
#include "util/hitbox.hpp"
#include "util/phys.hpp"
#include "util/safecall.hpp"
#include "util/error.hpp"
#include "util/log.hpp"
#include "util/file/yaml.hpp"
#include "game/util/game-archive.hpp"
#include "game/util/camera.hpp"
#include "game/util/vec-helper.hpp"
#include "game/util/resource-helper.hpp"
#include "game/core/time-scale.hpp"
#include "game/core/core.hpp"
#include "game/core/canvas.hpp"
#include "game/core/common.hpp"
#include "game/entity/util/collider-optimizer.hpp"
#include "game/entity/util/scatter.hpp"
#include "game/entity/util/entity-util.hpp"
#include "game/entity/enemy/cosmic-hunter.hpp"
#include "game/entity/enemy/cosmic-waiter.hpp"
#include "game/entity/enemy/cosmic.hpp"
#include "game/entity/enemy/illaen.hpp"
#include "game/entity/enemy/snake.hpp"
#include "game/entity/enemy/enemy-tutorial.hpp"

struct Entity_mgr::Impl {
  Entities m_entities {}; // массив всех игровых объектов
  Shared<Collider> m_collision_resolver {}; // обработчик столкновений
  Mem_pool m_entity_pool {}; // мем-пул под объекты
  Mem_pool m_phys_pool {}; // мем-пул под физические контексты
  Mem_pool m_hitbox_pool {}; // мем-пул под хитбоксы
  // БД инициализаторов объектов
  std::unordered_map<Str, Shared<Entity_loader>> m_entity_loaders {};
  Vector<Scatter> m_scatters {}; // источники взрывных волн
  Entities m_registrate_list {};
  // текущая ссылка на игрока, чтобы враги могли брать его в таргет
  Player* m_player {};
  bool m_visible {true}; // видимость игровых объектов
  Unique<Collider_optimizer> m_collider_optimizer {};
  Entity_mgr& m_master;
  bool m_update_lag {}; // true при слишком медленном апдейте
  Delta_time m_collider_time {}; // время на просчёт коллизий

  inline Impl(Entity_mgr& master): m_master {master} {
    #ifndef ECOMEM
    m_entities.reserve(4'000);
    #endif
    init_unique(m_collider_optimizer, m_master);
  }

  inline ~Impl() { clear(); }

  inline cr<Entities> get_entities() const { return m_entities; }

  inline void set_collider(cr<Shared<Collider>> new_collider)
    { m_collision_resolver = new_collider; }

  inline void draw(Image& dst, const Vec offset) const {
    return_if(!m_visible);
    // нарисовать нижний слой
    for (crauto entity: m_entities)
      if (entity->status.live && !entity->status.layer_up)
        entity->draw(dst, offset);
    // нарисовать верхний слой
    for (crauto entity: m_entities)
      if (entity->status.live && entity->status.layer_up)
        entity->draw(dst, offset);
  }

  inline void update(const Delta_time dt) {
    cauto st = hpw::get_time();

    accept_registrate_list();
    bound_check();
    update_kills();
    update_scatters();

    // просчёт столкновений
    m_collider_optimizer->update();
    if (m_collision_resolver) {
      cauto cld_st = hpw::get_time();
      (*m_collision_resolver)(m_entities, dt);
      m_collider_time = hpw::get_time() - cld_st;
    } else {
      m_collider_time = 0;
    }

    update_entitys(dt);

    cauto ed = hpw::get_time();
    m_update_lag = (ed - st) >= hpw::target_tick_time * 1.1;
  } // update
  
  // применить список на добавление объектов из очереди m_registrate_list
  inline void accept_registrate_list() {
    m_entities.reserve(m_entities.size() + m_registrate_list.size());
    for (rauto elem: m_registrate_list)
      m_entities.emplace_back(std::move(elem));
    m_registrate_list.clear();
  }

  // применить все взрывные волны к объектам
  inline void update_scatters() {
    if (!m_scatters.empty()) {
      for (rauto entity: m_entities) {
        if (entity->status.live && !entity->status.ignore_scatter) {
          for (crauto scatter: m_scatters)
            scatter.accept(*entity);
        }
      }
      m_scatters.clear();
    }
  }

  inline Entity* registrate(Entities::value_type&& entity) {
    assert(entity);
    auto ret = m_registrate_list.emplace_back(std::move(entity)).get();
    return ret;
  }

  inline void clear() {
    hpw_log("Entity_mgr.clear\n", Log_stream::debug);
    m_collision_resolver = {};
    clear_entities();
  }

  inline void clear_entities() {
    m_player = {};
    m_entities.clear();
    m_phys_pool.release();
    m_hitbox_pool.release();
    clear_entity_uid();
  }

  inline void update_entitys(const Delta_time dt) {
    assert(hpw::time_scale > 0);

    for (rauto entity: m_entities) {
      if (entity->status.live) {
        // изменять время для игрока и его объектов
        cauto player = get_player();
        if (entity.get() == player || entity->get_master() == player)
          entity->update(dt / hpw::time_scale);
        else // все остальные сущности
          entity->update(dt);
      }
    } // if live
  } // update_entitys

  inline void update_kills() {
    if (m_entities.empty())
      m_entity_pool.release();

    for (rauto entity: m_entities) {
      assert(entity);
      cont_if(entity->status.removed);

      // смерть от конца анимации
      cauto KILL_BY_END_ANIM = entity->status.kill_by_end_anim
        && entity->status.end_anim;
      // смерть от конца кадра
      cauto KILL_BY_END_FRAME = entity->status.kill_by_end_frame
        && entity->status.end_frame;
      if (KILL_BY_END_ANIM || KILL_BY_END_FRAME)
        entity->kill();

      // смерть от флага killme (нет жизней или конец анимации)
      if (entity->status.killme)
        entity->process_kill();
      // удалить помеченные (removed) объекты
      if (!entity->status.removed && entity->status.removeme)
        entity->process_remove();
    }
  } // update_kills

  inline void debug_draw(Image& dst) const {
    return_if (!m_collision_resolver);
    m_collision_resolver->debug_draw(dst, graphic::camera->get_offset());
  }

  inline Mem_pool& get_phys_pool() { return m_phys_pool; }
  inline Mem_pool& get_hitbox_pool() { return m_hitbox_pool; }
  inline Mem_pool& get_entity_pool() { return m_entity_pool; }

  // по type определяет какой Entity_loader создать и передать ему параметры с конфига
  inline Shared<Entity_loader> make_entity_loader(cr<Str> type, cr<Yaml> config) {
    // регистрация загрузчиков объектов
    using Maker = std::function< Shared<Entity_loader> (cr<Yaml>) >;
    std::unordered_map<Str, Maker> table {
      {"explosion", [](cr<Yaml> config){ return new_shared<Explosion_loader>(config); } },
      {"bonus", [](cr<Yaml> config){ return new_shared<Bonus_loader>(config); } },
      {"bullet", [](cr<Yaml> config){ return new_shared<Bullet_loader>(config); } },
      {"particle", [](cr<Yaml> config){ return new_shared<Particle_loader>(config); } },
      {"enemy.illaen", [](cr<Yaml> config){ return new_shared<Illaen::Loader>(config); } },
      {"enemy.cosmic.hunter", [](cr<Yaml> config){ return new_shared<Cosmic_hunter::Loader>(config); } },
      {"enemy.cosmic.waiter", [](cr<Yaml> config){ return new_shared<Cosmic_waiter::Loader>(config); } },
      {"enemy.cosmic", [](cr<Yaml> config){ return new_shared<Cosmic::Loader>(config); } },
      {"enemy.tutorial", [](cr<Yaml> config){ return new_shared<Enemy_tutorial::Loader>(config); } },
      {"snake.head", [](cr<Yaml> config){ return new_shared<Enemy_snake_head::Loader>(config); } },
      {"snake.tail", [](cr<Yaml> config){ return new_shared<Enemy_snake_tail::Loader>(config); } },
      {"player.boo.dark", [](cr<Yaml> config){ return new_shared<Player_dark::Loader>(config); } },
    };

    try {
      return table.at(type)(config);
    } catch (cr<std::out_of_range> ex) {
      error("тип загрузчика объекта \"" << type << "\" не зарегистрирован\n" << ex.what());
    }

    return {};
  } // make_entity_loader

  inline void register_types() {
    m_entity_loaders.clear();

    #ifndef ECOMEM // при экономии памяти объекты подгружаются в момент вызова
      // загрузить все объекты из конфига
      auto config = load_entity_config();
      for (crauto entity_name: config.root_tags()) {
        auto entity_node = config[entity_name];
        auto type = entity_node.get_str("type", "error type");
        m_entity_loaders[entity_name] = make_entity_loader(type, entity_node);
      }
    #endif
  } // register_types

  inline Entity* load_unknown_entity(Entity* master, cr<Str> name, const Vec pos) {
    // попытаться загрузить отсутствующий объект
    auto config = load_entity_config();
    auto entity_node = config[name];
    auto type = entity_node.get_str("type", "error type");
    m_entity_loaders[name] = make_entity_loader(type, entity_node);
    Entity_loader* loader;
      
    try {
      cauto ptr = m_entity_loaders.at(name);
      loader = ptr.get();
    } catch (cr<std::out_of_range> ex) {
      error("нет инициализатора для \"" << name << "\"");
    }

    assert(loader);
    return (*loader)(master, pos);
  }

  inline Entity* make(Entity* master, cr<Str> name, const Vec pos) {
    #ifdef ECOMEM
      return load_unknown_entity(master, name, pos);
    #endif

    iferror(m_entity_loaders.empty(),
      "вызови register_types для заполнения m_entity_loaders");

    try {
      auto entity_loader = m_entity_loaders.at(name);
      assert(entity_loader);
      auto ret = (*entity_loader)(master, pos);
      if (ret)
        ret->set_name(name);
      return ret;
    } catch (cr<std::out_of_range> ex) {
      //#ifdef ECOMEM
        return load_unknown_entity(master, name, pos);
      //#else
        //error("нет инициализатора для \"" << name << "\"");
      //#endif
    }
    
    return {};
  } // make

  inline Yaml load_entity_config() const {
    return Yaml(load_res("config/entities.yml"));
  }

  inline void add_scatter(cr<Scatter> scatter) {
    return_if(scatter.power <= 0);
    return_if(scatter.range <= 0);
    m_scatters.emplace_back(scatter);
  }

  // удаление объектов за экраном
  inline void bound_check() {
    for (rauto entity: m_entities) {
      cont_if(!entity->status.live);
      cont_if(entity->status.ignore_bound);

      crauto entity_pos = entity->phys.get_pos();
      auto bound = entity->status.is_bullet
        ? hpw::shmup_bound_for_bullet
        : hpw::shmup_bound_for_other;

      if (
        // тихо убить объект за экраном
        (entity->status.remove_by_out_of_screen && entity->status.out_of_screen) ||
        // тихо убить объект, если он вышел за пределы
        entity_pos.x <= -bound ||
        entity_pos.x >= graphic::width + bound ||
        entity_pos.y <= -bound ||
        entity_pos.y >= graphic::height + bound
      ) {
        entity->remove();
      }
    } // for m_entities
  } // bound_check

  // возвращает первый попавшийся мёртвый объект нужного типа
  inline Entity* find_avaliable_entity(const Entity_type type) {
    for (rauto entity: get_entities())
      if (entity->type == type && !entity->status.live)
        return entity.get();
    return {};
  }

  inline Player* get_player() const { return m_player; }
  inline void set_player(Player* player) { m_player = player; }

  Vec target_for_enemy() const {
    // если игрок есть
    if (cauto player = get_player(); player) {
      // если игрок не невидимый TODO
      //if (!player->find_ability(Ability_invise(*player).type_id()))
        return player->phys.get_pos();
    }
    
    // если игрока не нашли, стрелять куда попало
    return get_rand_pos_safe(0, 0, graphic::width, graphic::height);
  }

  inline void set_visible(const bool mode) { m_visible = mode; }

  inline Entity* find(const Uid uid) const {
    auto it = std::find_if(m_entities.begin(), m_entities.end(),
      [&](cr<decltype(m_entities)::value_type> entity)
      { return entity->status.live && entity->uid == uid; }
    );
    if (it != m_entities.end())
      return it->get();
    return nullptr;
  }

  inline uint lives() const {
    uint count {};
    for (crauto ent: m_entities)
      count += ent->status.live;
    return count;
  }

  inline std::size_t entity_loaders_sz() const {
    return m_entity_loaders.size();
  }

  bool update_lag() const { return m_update_lag; }
  Delta_time collider_time() const { return m_collider_time; }

  Entity* get_entity(const Uid uid) const {
    auto it = std::find_if(m_entities.begin(), m_entities.end(),
      [uid](cr<decltype(m_entities)::value_type> ent) {
        return_if(!ent, false);
        return_if(!ent->status.live, false);
        return ent->uid == uid;
      }
    );
    return it == m_entities.end() ? nullptr : it->get();
  }
}; // Impl

Entity_mgr::Entity_mgr(): impl {new_unique<Impl>(*this)} {}
Entity_mgr::~Entity_mgr() { impl->clear(); }
void Entity_mgr::draw(Image& dst, const Vec offset) const { impl->draw(dst, offset); }
void Entity_mgr::update(const Delta_time dt) { impl->update(dt); }
void Entity_mgr::clear() { impl->clear(); }
void Entity_mgr::clear_entities() { impl->clear_entities(); }
void Entity_mgr::set_collider(cr<Shared<Collider>> new_collider) { impl->set_collider(new_collider); }
void Entity_mgr::register_types() { impl->register_types(); }
Entity* Entity_mgr::make(Entity* master, cr<Str> name, const Vec pos) { return impl->make(master, name, pos); }
Entity* Entity_mgr::registrate(Entities::value_type&& entity) { return impl->registrate(std::move(entity)); }
void Entity_mgr::add_scatter(cr<Scatter> scatter) { return impl->add_scatter(scatter); }
void Entity_mgr::debug_draw(Image& dst) const { impl->debug_draw(dst); }
Mem_pool& Entity_mgr::get_phys_pool() { return impl->get_phys_pool(); }
Mem_pool& Entity_mgr::get_hitbox_pool() { return impl->get_hitbox_pool(); }
Mem_pool& Entity_mgr::get_entity_pool() { return impl->get_entity_pool(); }
cr<Entities> Entity_mgr::get_entities() const { return impl->get_entities(); }
Entity* Entity_mgr::find_avaliable_entity(const Entity_type type) { return impl->find_avaliable_entity(type); }
Player* Entity_mgr::get_player() const { return impl->get_player(); }
void Entity_mgr::set_player(Player* player) { impl->set_player(player); }
Vec Entity_mgr::target_for_enemy() const { return impl->target_for_enemy(); }
void Entity_mgr::set_visible(const bool mode) { return impl->set_visible(mode); }
Entity* Entity_mgr::find(const Uid uid) const { return impl->find(uid); }
uint Entity_mgr::lives() const { return impl->lives(); }
std::size_t Entity_mgr::entity_loaders_sz() const { return impl->entity_loaders_sz(); }
bool Entity_mgr::update_lag() const { return impl->update_lag(); }
Delta_time Entity_mgr::collider_time() const { return impl->collider_time(); }
Entity* Entity_mgr::get_entity(const Uid uid) const { return impl->get_entity(uid); }
