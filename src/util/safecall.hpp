#pragma once

// вызов функции с проверкой её валидности
inline void safecall(auto func, auto ...args) {
  if (func)
    func(args...);
}

// вызов функции с проверкой её валидности и получение возврата
template <class RET>
inline RET saferet(auto func, auto ...args) {
  if (func)
    return func(args...);
  return RET{};
}
