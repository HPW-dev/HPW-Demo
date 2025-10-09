#pragma once

#define cauto const auto
#define scauto static const auto
#define crauto const auto&
#define cpauto const auto*
#define rauto auto&
#define sconst static const
#define constx constexpr static const
#define cautox constexpr static const auto

template <class T>
using cr = const T&;

template <class T>
using cp = const T*;

template <class T>
inline constexpr T scast(auto&& val) { return static_cast<T>(val); }

template <class T>
inline constexpr T dcast(auto&& val) { return dynamic_cast<T>(val); }

template <class T>
inline constexpr T rcast(auto&& val) { return reinterpret_cast<T>(val); }

template <class T>
inline constexpr T ccast(auto&& val) { return const_cast<T>(val); }

template <class T>
inline constexpr T ptr2ptr(auto* src) { return scast<T>( scast<void*>(src) ); }

template <class T>
inline constexpr T cptr2ptr(auto* src) { return scast<T>( scast<cp<void>>(src) ); }

// for с автоопределением типа индекса
#define cfor(index_name, sz) \
for (auto index_name = decltype(sz){}; index_name < sz; ++index_name)

// применяет expr для container
#define cppfor(container, expr) \
for (auto& val: container) \
  val->expr;

#define return_if(cond, ...) \
do { \
  if (static_cast<bool>(cond)) { \
    return __VA_ARGS__; \
  } \
} while (false);
#define ret_if return_if
#define break_if(cond) if (cond) break;
#define continue_if(cond) if (cond) continue;
#define cont_if continue_if
#define elif else if

// макрос для классов запрещающий копирование
#define nocopy(name) \
name(name&&) = delete; \
name(cr<name>) = delete; \
name& operator = (name&&) = delete; \
name& operator = (cr<name>) = delete;

// для обмана оптимизатора
#if defined(__clang__)
  [[clang::optnone]] void do_not_optimize(auto val) { (void)val; }
#else // GCC:
  #pragma GCC push_options
  #pragma GCC optimize ("O0")
  #define do_not_optimize(val) { (void)val; }
  #pragma GCC pop_options
#endif

// вспомогательный макрос для CONCAT
#define _CONCAT(a, b) a##b
// объединяем названия содержащиеся в макросах
#define CONCAT(a, b) _CONCAT(a, b)

// вспомогательный макрос STRINGIFY
#define _STRINGIFY(x) #x
// превращает название в макросе в Си-строку
#define STRINGIFY(x) _STRINGIFY(x)
