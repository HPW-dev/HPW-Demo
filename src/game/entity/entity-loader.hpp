#pragma once
#include "util/macro.hpp"

class Entity;
struct Vec;

// база для инициализаторов объектов
class Entity_loader {
protected:
  // helper-функция для загрузчиков
  static void prepare(Entity& dst, Entity* master, const Vec pos);

public:
  Entity_loader() = default;
  virtual ~Entity_loader() = default;
  // выделяет память под объект и подготавливает его данные
  // @param master кто создал объект
  // @param pos где спавнить объект
  // @param parent если память уже выделена извне
  // @return ссыль на управляющий объект
  virtual Entity* operator()(Entity* master, const Vec pos, Entity* parent={}) = 0;
};
