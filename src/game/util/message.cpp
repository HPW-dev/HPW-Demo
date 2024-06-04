#include "message.hpp"

struct Message_mgr::Impl {
  bool m_visible {true};

  inline void move(Message&& msg) {}
  inline void update(const Delta_time dt) {}
  inline void draw(Image& dst) const {}
  inline CN<Messages> messages() const {}
  inline void clear() {}
  inline void set_visible(const bool enable) { m_visible = enable; }
};

Message_mgr::Message_mgr(): impl{new_unique<Impl>()} {}
Message_mgr::~Message_mgr() {}
void Message_mgr::move(Message&& msg) { impl->move(std::move(msg)); }
void Message_mgr::update(const Delta_time dt) { impl->update(dt); }
void Message_mgr::draw(Image& dst) const { impl->draw(dst); }
CN<Message_mgr::Messages> Message_mgr::messages() const { impl->messages(); }
void Message_mgr::clear() { impl->clear(); }
void Message_mgr::set_visible(const bool enable) { impl->set_visible(enable); }
