#pragma once
#include "util/macro.hpp"

class Entity;
class Image;
struct Vec;

// даёт игровым объектам дебажные функции
class Entity_debug {
public:
  inline explicit Entity_debug(Entity& master): _master {master} {}

protected:
  void draw_pos(Image& dst, const Vec offset) const;
  void debug_draw(Image& dst, const Vec offset) const;

private:
  nocopy(Entity_debug);
  Entity& _master;
};
