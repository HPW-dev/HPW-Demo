#include <utility>
#include "hpw-util.hpp"

Scope::Scope(Func&& ctor_fn, Func&& dtor_fn)
: m_ctor_fn {std::move(ctor_fn)}
, m_dtor_fn {std::move(dtor_fn)}
{
  if (m_ctor_fn)
    m_ctor_fn();
}

Scope::~Scope() {
  if (m_dtor_fn)
    m_dtor_fn();
}
