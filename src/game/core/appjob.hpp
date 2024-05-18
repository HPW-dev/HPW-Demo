#pragma once
#include <functional>
#include <deque>

using App_job = std::function<void (const double dt)>;

namespace hpw {
// исполняет задачи на самом верхнем уровне
inline std::deque<App_job> app_job {};
}
