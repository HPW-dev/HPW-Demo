#pragma once

#ifndef ECOMEM
#include <memory_resource>
#include <functional>
#endif

#include <memory>
#include <utility>
#include "util/mem-types.hpp"
#include "util/vector-types.hpp"
#include "util/macro.hpp"

#ifdef ECOMEM
  #define Pool_ptr(T) Shared<T>

#else
  // заглушка за место Shared
  template <class T>
  class _Pool_ptr final {
    T* ptr {};

  public:
    _Pool_ptr() = default;
    ~_Pool_ptr() = default;

    template <class T2>
    inline _Pool_ptr(T2* other) noexcept
    : ptr {other} {}

    template <class T2>
    inline _Pool_ptr* operator =(cr<_Pool_ptr<T2>> other) noexcept
    { ptr = other.get(); return this; }

    template <class T2>
    inline _Pool_ptr* operator =(_Pool_ptr<T2>&& other) noexcept
    { ptr = other.get(); other = {}; return this; }

    template <class T2>
    inline _Pool_ptr(cr<_Pool_ptr<T2>> other) noexcept
    : ptr {other.get()} {}

    template <class T2>
    inline _Pool_ptr(_Pool_ptr<T2>&& other) noexcept
    : ptr {other.get()}
    { other = {}; }

    inline T* get() const { return ptr; }
    inline T* operator->() const { return ptr; }
    inline T& operator*() const { return *ptr; }
    inline operator bool() const { return scast<bool>(ptr); }
  }; // _Pool_ptr

  #define Pool_ptr(T) _Pool_ptr<T> 
#endif

class Mem_pool {
  void add_used_bytes(std::size_t sz);
  void sub_used_bytes(std::size_t sz);
  void print_used_bytes();
  std::size_t m_allocated {};

public:
  #ifndef ECOMEM
  std::pmr::monotonic_buffer_resource source {};
  Vector<std::function<void ()>> deleters {};
  #endif

  explicit Mem_pool(std::size_t chunk_sz = 256*256);
  ~Mem_pool();
  void release();
  inline std::size_t allocated() const { return m_allocated; }

  template <class T, typename... Args>
  inline Pool_ptr(T) new_object(Args&&... args) {
    m_allocated += sizeof(T);

    #ifdef ECOMEM
      auto ret = new_shared<T>(std::forward<Args>(args)...);
    #else
      std::pmr::polymorphic_allocator<T> pa {&source};
      auto ret = pa.template new_object<T>(std::forward<Args>(args)...);
      add_used_bytes(sizeof(T));

      // чтобы деструктор вызывался корректно
      deleters.emplace_back ( [this, ret] {
        std::pmr::polymorphic_allocator<T> pa {&source};
        pa.delete_object(ret);
        sub_used_bytes(sizeof(T));
      } );
    #endif
    return ret;
  }
}; // Mem_pool
