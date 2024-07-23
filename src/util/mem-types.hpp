#pragma once
#include <memory>

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
Shared<T> new_shared(Args&&... args) { return Shared<T>(new T(args...)); }

// выделяет данные в std::unique совместимом поинтере
template <class T, typename... Args>
Unique<T> new_unique(Args&&... args) { return Unique<T>(new T(args...)); }

// инициализирует данные в std::unique совместимом поинтере
template <typename... Args>
void init_unique(auto& ptr, Args&&... args) { ptr = new_unique(args...); }

// инициализирует данные в std::shared совместимом поинтере
template <typename... Args>
void init_shared(auto& ptr, Args&&... args) { ptr = new_shared(args...); }
