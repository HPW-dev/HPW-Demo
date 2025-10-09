#pragma once
#include <functional>
#include <typeinfo>
#include "util/macro.hpp"
#include "util/mem-types.hpp"
#include "util/str.hpp"
#include "util/math/num-types.hpp"
#include "graphic/image/image-fwd.hpp"

namespace bgp {

// база для создания фонового узора
class Bgp {
public:
  Bgp() = default;
  virtual ~Bgp() = default;
  inline virtual void restart() {}
  inline virtual void update(Delta_time dt) {}
  virtual void draw(Image& dst) const = 0;
};

using Maker = std::function< Shared<Bgp>() >; // конструктор фоновых узоров
[[nodiscard]] Shared<Bgp> make(cr<Str> name); // получить фоновой узор по имениs
[[nodiscard]] Strs all_names();               // узнать какие есть названия фонов
void registrate(cr<Str> name, Maker maker);   // добавить фон в базу фонов

// класс для автодобавления узора в список узоров
template <class BGP> struct Registrator {
  inline Registrator() { registrate(typeid(BGP).name(), []->Maker { return new_shared<BGP>(); } ); }
};

} // bgp ns
