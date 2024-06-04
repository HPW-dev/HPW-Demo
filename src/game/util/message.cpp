#include <cassert>
#include <algorithm>
#include <utility>
#include "message.hpp"
#include "util/log.hpp"
#include "game/core/fonts.hpp"
#include "graphic/font/font.hpp"
#include "graphic/image/image.hpp"

struct Message_mgr::Impl {
  struct Message_ctx {
    Message msg {};
    Delta_time lifetime {};
  };
  Vector<Message_ctx> m_messages {};
  bool m_visible {true};

  inline void move(Message&& msg) {
    test_message(msg);
    m_messages.emplace_back( Message_ctx {
      .msg = std::move(msg),
      .lifetime = msg.lifetime
    } );
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
    utf32 concated;
    for (cnauto ctx: m_messages) {
      concated += ctx.msg.text;
      if (ctx.msg.text_gen)
        concated += ctx.msg.text_gen();
      concated += U'\n';
    }
    graphic::font->draw(dst, {5, 5}, concated, &blend_diff);
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
}; // Impl

Message_mgr::Message_mgr(): impl{new_unique<Impl>()} {}
Message_mgr::~Message_mgr() {}
void Message_mgr::move(Message&& msg) { impl->move(std::move(msg)); }
void Message_mgr::update(const Delta_time dt) { impl->update(dt); }
void Message_mgr::draw(Image& dst) const { impl->draw(dst); }
void Message_mgr::clear() { impl->clear(); }
void Message_mgr::set_visible(const bool enable) { impl->set_visible(enable); }
