#pragma once

// вызов функции с проверкой её валидности
inline void safecall(auto func, auto ...args) {
  if (func)
    func(args...);
}

// вызов функции с проверкой её валидности и получение возврата
inline auto saferet(auto func, auto ...args) {
  if (func)
    return func(args...);
  return {};
}
