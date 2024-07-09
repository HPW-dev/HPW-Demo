#pragma once
#include <utility>
#include "util/mem-types.hpp"
#include "util/math/num-types.hpp"
#include "util/str.hpp"
#include "util/macro.hpp"
#include "util/mempool.hpp"
#include "game/entity/entity-type.hpp"
#include "game/entity/entity.hpp"

class Collider;
class Entity;
class Phys;
class Hitbox;
class Player;
class Image;
struct Vec;
struct Scatter;

// управление игровыми объектами
class Entity_mgr final {
  nocopy(Entity_mgr);
  struct Impl;
  Unique<Impl> impl {};

  // найти в памяти подходящий по типу объект
  Entity* find_avaliable_entity(const Entity_type type);
  // добавить объект в систему
  Entity* registrate(Entities::value_type&& entity);

public:
  Entity_mgr();
  ~Entity_mgr();

  void draw(Image& dst, const Vec offset) const;
  void update(const Delta_time dt);
  void clear();
  // убирает все объекты
  void clear_entities();
  void debug_draw(Image& dst) const;
  // mode=false выключает видимость игровых объектов
  void set_visible(const bool mode);

  // задать обработчик столкновений
  void set_collider(CN<Shared<Collider>> new_collider);
  // вызвать это перед использованием make
  void register_types();
  /** Создаёт объкт в памяти на указанном месте
  @param master родитель объекта, используется объектом для внутренней логики
  @param name имя объекта из базы
  @param pos соспавнить объект на этой позиции
  @return по возможности верёт объект, для последующего изменения извне */
  Entity* make(Entity* master, CN<Str> name, const Vec pos);
  // найти объект по его UID. Null при ошибке
  Entity* find(const Uid uid) const;
  // создать волну от взрыва расталкивающую объекты
  void add_scatter(CN<Scatter> scatter);
  Mem_pool& get_phys_pool();
  Mem_pool& get_hitbox_pool();
  Mem_pool& get_entity_pool();
  // получить массив всех объектов
  CN<Entities> get_entities() const;
  uint lives() const; // сколько сейчас активных объектов
  // ссылка на игрока
  Player* get_player() const;
  void set_player(Player* player);
  // сюда должны стрелять противники
  Vec target_for_enemy() const;
  
  // выделить память под объект
  template <class T> inline T* allocate();
}; // Entity_mgr

template <class T> inline T* Entity_mgr::allocate() {
  // поиск объекта в такого же типа, но мёртвого
  if (auto it = find_avaliable_entity(ENTITY_TYPE(T)); it) {
    auto ret = ptr2ptr<T*>(it);
    ret->~T(); // вызвать декструктор для корректной работы
    new (ret) T(); // воскресить объект
    return ret;
  }

  // если не нашли, то выделить новую память и зарегистрировать объект
  auto allocated = get_entity_pool().new_object<T>();
  #ifdef ECOMEM
  return ptr2ptr<T*>(registrate(std::move(allocated)) );
  #else
  return ptr2ptr<T*>(registrate(std::move(allocated.get())) );
  #endif
}
