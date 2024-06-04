#include <cassert>
#include <algorithm>
#include <utility>
#include "message.hpp"
#include "util/log.hpp"
#include "game/core/fonts.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/util/graphic-util.hpp"

struct Message_mgr::Impl {
  Messages m_messages {};
  bool m_visible {true};

  inline void move(Message&& msg) {
    test_message(msg);
    m_messages.emplace_back( std::move(msg) );
  }

  inline void update(const Delta_time dt) {
    // обновить таймер жизни сообщений и удалить истёкшие
    std::erase_if(m_messages, [dt](nauto msg) {
      bool ret = msg.lifetime <= 0;
      msg.lifetime -= dt;
      return ret;
    });
  }

  inline void draw(Image& dst) const {
    // определить какую область занимают все сообщения
    utf32 concated;
    for (cnauto msg: m_messages) {
      concated += msg.text;
      if (msg.text_gen)
        concated += msg.text_gen();
      concated += U'\n';
    }
    const Vec pos = center_point(
      {dst.X, dst.Y}, graphic::font->text_size(concated));

    graphic::font->draw(dst, pos, concated, &blend_diff);
  }

  // проверяет сообщение на валидность
  inline void test_message(CN<Message> msg) const {
    assert(!msg.text.empty() || msg.text_gen);
    assert(msg.blink_delay >= 0 && msg.blink_delay < 1000u);
    assert(msg.lifetime > 0);
    iflog(msg.color != Pal8::white, "Message_mgr: need impl for other colors\n");
  }

  inline void clear() { m_messages.clear(); }
  inline void set_visible(const bool enable) { m_visible = enable; }
  inline CN<Messages> messages() const { return m_messages; }
}; // Impl

Message_mgr::Message_mgr(): impl{new_unique<Impl>()} {}
Message_mgr::~Message_mgr() {}
void Message_mgr::move(Message&& msg) { impl->move(std::move(msg)); }
void Message_mgr::update(const Delta_time dt) { impl->update(dt); }
void Message_mgr::draw(Image& dst) const { impl->draw(dst); }
void Message_mgr::clear() { impl->clear(); }
void Message_mgr::set_visible(const bool enable) { impl->set_visible(enable); }
CN<Message_mgr::Messages> Message_mgr::messages() const { return impl->messages(); }
