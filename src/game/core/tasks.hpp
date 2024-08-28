#pragma once
#include "util/mem-types.hpp"
#include "game/util/task.hpp"

namespace hpw {
inline Task_mgr task_mgr {}; // задачи встраиваемые в геймлуп (игровой сцены)
inline Task_mgr global_task_mgr {}; // задачи встраиваемые глобально в игру
}
