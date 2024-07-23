#pragma once
#include <memory>
#include <type_traits>

template <class T>
using Shared = std::__shared_ptr<T, __gnu_cxx::_S_single>;
//using Shared = std::shared_ptr<T>; clang

template <class T>
using Weak = std::__weak_ptr<T, __gnu_cxx::_S_single>;
//using Weak = std::weak_ptr<T>; clang

template <class T>
using Unique = std::unique_ptr<T>;

// выделяет данные в std::shared совместимом поинтере
template <class T, typename... Args>
[[nodiscard]] inline Shared<T> new_shared(Args&&... args) {
  return Shared<T>(new T(args...));
}

// выделяет данные в std::unique совместимом поинтере
template <class T, typename... Args>
[[nodiscard]] inline Unique<T> new_unique(Args&&... args) {
  return Unique<T>(new T(args...));
}

// инициализирует данные в std::unique_ptr совместимом поинтере
template <class T, typename... Args>
inline void init_unique(auto& ptr, Args&&... args) {
  ptr = new_unique<T>(args...);
}

// инициализирует данные в std::shared_ptr совместимом поинтере
template <class T, typename... Args>
inline void init_shared(auto& ptr, Args&&... args) {
  ptr = new_shared<T>(args...);
}

// инициализирует данные в std::unique_ptr совместимом поинтере
template <typename... Args>
inline void init_unique(auto& ptr, Args&&... args) {
  using T = std::remove_reference_t<decltype(ptr)>::element_type;
  ptr = new_unique<T>(args...);
}

// инициализирует данные в std::shared_ptr совместимом поинтере
template <typename... Args>
inline void init_shared(auto& ptr, Args&&... args) {
  using T = std::remove_reference_t<decltype(ptr)>::element_type;
  ptr = new_shared<T>(args...);
}
