#pragma once
#include "util/mem-types.hpp"

// игровые логи на экране
class Message_mgr {
  struct Impl;
  Unique<Impl> impl {};

public:
  Message_mgr();
  ~Message_mgr();
};
