#pragma once
#include <utility>
#include <functional>
#include <deque>
#include "util/math/num-types.hpp"
#include "util/mem-types.hpp"
#include "util/unicode.hpp"
#include "graphic/image/color-blend.hpp"
#include "graphic/image/image-fwd.hpp"

// сообщение для игрового лога на экране
struct Message {
  using Text_gen = std::function< utf32 () >;
  using Action = std::function< void () >;

  Delta_time lifetime {2.5}; // сколько секунд показывать сообщение
  utf32 text {}; // основное сообщение
  Text_gen text_gen {}; // колбэк на получение сообщения
  Action on_end_action {}; // колбэк при завершении показа сообщения
  blend_pf bf {&blend_diff}; // режим смешивания пикселей для сообщения
  Pal8 color {Pal8::white};
  uint blink_delay {}; // если > 1, то сообщение будет мерцать с указанной задержкой
};

// игровые логи на экране
class Message_mgr {
  struct Impl;
  Unique<Impl> impl {};
  using Messages = std::deque<Message>;

public:
  Message_mgr();
  ~Message_mgr();
  // добавить сообщение
  void move(Message&& msg);
  void update(const Delta_time dt);
  void draw(Image& dst) const;
  // получить все сообщения
  cr<Messages> messages() const;
  // удалить все сообщения
  void clear();
  // управление видимостью сообщений
  void set_visible(const bool enable);
};
