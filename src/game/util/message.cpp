#include "message.hpp"

struct Message_mgr::Impl {
  inline Impl() = default;
};

Message_mgr::Message_mgr(): impl{new_unique<Impl>()} {}
Message_mgr::~Message_mgr() {}
