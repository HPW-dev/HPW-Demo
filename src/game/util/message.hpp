#pragma once
#include <functional>
#include "util/math/num-types.hpp"
#include "util/mem-types.hpp"
#include "util/unicode.hpp"

class Image;

// сообщение для игрового лога на экране
struct Message {

};

// игровые логи на экране
class Message_mgr {
  struct Impl;
  Unique<Impl> impl {};
  using Messages = Vector<Message>;

public:
  Message_mgr();
  ~Message_mgr();
  // добавить сообщение
  void move(Message&& msg);
  void update(const Delta_time dt);
  void draw(Image& dst) const;
  // получить все сообщения
  CN<Messages> messages() const;
  // удалить все сообщения
  void clear();
  // управление видимостью сообщений
  void set_visible(const bool enable);
};
