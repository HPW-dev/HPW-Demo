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

template <class T, typename... Args>
Shared<T> new_shared(Args&&... args) { return Shared<T>(new T(args...)); }

template <class T, typename... Args>
Unique<T> new_unique(Args&&... args) { return Unique<T>(new T(args...)); }
