#include "mempool.hpp"

#ifndef ECOMEM
  #include <atomic>
  #include "util/log.hpp"
  #include "util/str-util.hpp"

  inline std::atomic_uint mem_pool_cur_bytes_used {0};
  inline std::atomic_uint mem_pool_max_bytes_used {0};
#endif

Mem_pool::Mem_pool(std::size_t chunk_sz)
#ifndef ECOMEM
: source(chunk_sz)
#endif
{}

void Mem_pool::release() {
#ifndef ECOMEM
  for (crauto deleter: deleters)
    deleter();
  deleters.clear();
  source.release();
#endif

  m_allocated = 0;
}

Mem_pool::~Mem_pool() {
#ifndef ECOMEM
  release();
  print_used_bytes();
#endif
}

void Mem_pool::add_used_bytes(std::size_t sz) {
#ifndef ECOMEM
  mem_pool_cur_bytes_used += sz;
  mem_pool_max_bytes_used.store( std::max(
    mem_pool_max_bytes_used,
    mem_pool_cur_bytes_used )
  );
#endif
}

void Mem_pool::sub_used_bytes(std::size_t sz) {
#ifndef ECOMEM
  mem_pool_cur_bytes_used -= sz;
#endif
}

void Mem_pool::print_used_bytes() {
#ifndef ECOMEM
  hpw_log(Str("max mem usage in pool: ") + n2s(mem_pool_max_bytes_used.load()) + " (" +
    n2s(scast<real>(mem_pool_max_bytes_used) / (1024*1024), 3) + " mb)\n", Log_stream::debug);
#endif
}

