#pragma once
#include "util/mem-types.hpp"
#include "util/str.hpp"
#include "util/macro.hpp"

class Scene;
// создать сцену по имени
Shared<Scene> find_scene(cr<Str> name);
// узнать имена всех доступных для поиска сцен
Strs scene_names();
