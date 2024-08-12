#pragma once

#define cauto const auto
#define scauto static const auto
#define cnauto const auto&
#define nauto auto&
#define sconst static const
#define constx constexpr static const
#define cautox constexpr static const auto

template <class T>
inline constexpr T scast(auto&& val) { return static_cast<T>(val); }

template <class T>
inline constexpr T dcast(auto&& val) { return dynamic_cast<T>(val); }

template <class T>
inline constexpr T rcast(auto&& val) { return reinterpret_cast<T>(val); }

template <class T>
inline constexpr T ccast(auto&& val) { return const_cast<T>(val); }

template <class T>
inline constexpr T ptr2ptr(auto* src) { return static_cast<T>( static_cast<void*>(src) ); }

template <class T>
inline constexpr T cptr2ptr(auto* src) { return static_cast<T>( static_cast<const void*>(src) ); }

template <class T>
using CN = const T&;

template <class T>
using CP = const T*;

inline constexpr const char* s2yn(const bool cond) { return cond ? "yes" : "no"; }

// for с авто определением типа индекса
#define cfor(index_name, maximum) \
for (auto index_name = decltype(maximum){0}; index_name < maximum; ++index_name)

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
name(CN<name>) = delete; \
name& operator = (name&&) = delete; \
name& operator = (CN<name>) = delete;

// для обмана оптимизатора
#if defined(__clang__)
  [[clang::optnone]] void do_not_optimize(auto val) { (void)val; }
#else // GCC:
  #pragma GCC push_options
  #pragma GCC optimize ("O0")
  #define do_not_optimize(val) { (void)val; }
  #pragma GCC pop_options
#endif
