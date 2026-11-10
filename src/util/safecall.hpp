#pragma once
#include <type_traits>
#include <utility>

#ifdef DEBUG
  #include "log.hpp"
#endif


// Безопасный вызов функции с проверкой, что она есть
template <typename F, typename... Args>
inline auto safe_call(F&& func, Args&&... args) -> std::invoke_result_t<F, Args...> {
  using RET = std::invoke_result_t<F, Args...>;

  // чекаем что функция есть
  if constexpr (std::is_constructible_v<bool, F>) {
    if (!func) {
      #ifdef DEBUG
        log_error << "safe_call - func is null";
      #endif
            
      // Если RET это void, просто выходим. Иначе возвращаем дефолтное RET{}
      if constexpr (std::is_void_v<RET>) {
        return; 
      } else {
        return RET{};
      }
    }
  }

  // просто запуск функции
  return std::forward<F>(func)(std::forward<Args>(args)...);
}
