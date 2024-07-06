#pragma once
#include <typeinfo>
#include "util/vector-types.hpp"
#include "util/mempool.hpp"
#include "util/math/num-types.hpp"

using Entity_type = std::size_t;

class Entity;
using Entitiess = Vector<Pool_ptr(Entity)>;

using hp_t = i64_t; // for hp/dmg/mana

#define ENTITY_TYPE(T) typeid(T).hash_code()
#define GET_SELF_TYPE ENTITY_TYPE(decltype(*this))
