#include <cassert>
#include <algorithm>
#include <utility>
#include "message.hpp"
#include "util/log.hpp"
#include "game/core/fonts.hpp"
#include "game/core/core.hpp"
#include "game/util/sync.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/graphic-util.hpp"

struct Message_mgr::Impl {
  constx uint LINE_LIMIT = 22;
  Messages m_messages {};
  bool m_visible {true};

  inline void move(Message&& msg) {
    test_message(msg);
    // показывать только определённое число сообщений на экране и не показывать старые
    if (m_messages.size() >= LINE_LIMIT) {
      m_messages.pop_front();
    }
    m_messages.emplace_back( std::move(msg) );
  }

  inline void update(const Delta_time dt) {
    assert(dt == hpw::target_tick_time);

    // обновить таймер жизни сообщений и удалить истёкшие
    std::erase_if(m_messages, [dt](rauto msg) {
      bool ret = msg.lifetime <= 0;
      msg.lifetime -= dt;
      if (ret && msg.on_end_action)
        msg.on_end_action();
      return ret;
    });
  }

  inline void draw(Image& dst) const {
    return_if(!m_visible);

    // определить какую область занимают все сообщения
    utf32 concated;
    for (crauto msg: m_messages) {
      concated += msg.text;
      if (msg.text_gen)
        concated += msg.text_gen();
      concated += U'\n';
    }
    Vec pos = center_point(
      {dst.X, dst.Y}, graphic::font->text_size(concated));

    // нарисовать каждую строку со своими настройками
    for (crauto msg: m_messages) {
      auto text = msg.text;
      if (msg.text_gen)
        text += msg.text_gen();
      // определить мигать текстом, или нет
      bool draw_text = msg.blink_delay == 0;
      if (msg.blink_delay > 1)
        draw_text = (graphic::frame_count % msg.blink_delay) >= (msg.blink_delay / 2);
      if (draw_text)
        graphic::font->draw(dst, pos, text, msg.bf, graphic::frame_count);
      pos.y += graphic::font->text_height(text);
    }
  } // draw

  // проверяет сообщение на валидность
  inline void test_message(cr<Message> msg) const {
    assert(!msg.text.empty() || msg.text_gen);
    assert(msg.blink_delay < 1000u);
    assert(msg.blink_delay == 0 || msg.blink_delay > 1);
    assert(msg.lifetime > 0);
    if (msg.color != Pal8::white)
      hpw_log("Message_mgr: need impl for other colors\n");
  }

  inline void clear() { m_messages.clear(); }
  inline void set_visible(const bool enable) { m_visible = enable; }
  inline cr<Messages> messages() const { return m_messages; }
}; // Impl

Message_mgr::Message_mgr(): impl{new_unique<Impl>()} {}
Message_mgr::~Message_mgr() {}
void Message_mgr::move(Message&& msg) { impl->move(std::move(msg)); }
void Message_mgr::update(const Delta_time dt) { impl->update(dt); }
void Message_mgr::draw(Image& dst) const { impl->draw(dst); }
void Message_mgr::clear() { impl->clear(); }
void Message_mgr::set_visible(const bool enable) { impl->set_visible(enable); }
cr<Message_mgr::Messages> Message_mgr::messages() const { return impl->messages(); }
