#include <imgui.h>
#include "window-emitter.hpp"
#include "entity-editor-ctx.hpp"
#include "util/log.hpp"

namespace {
Uid g_wnd_uid {}; // чтобы ImGui не перепутал окна
}

struct Window_emitter::Impl {
  Entity_editor_ctx& m_ctx;
  Uid m_uid {};

  inline explicit Impl(Entity_editor_ctx& ctx)
  : m_ctx {ctx} {
    m_uid = get_uid();
    hpw_log("добавлено окно эмиттера (№" << m_uid << ")\n");
  }

  inline void imgui_exec() {
  }

  inline static Uid get_uid() { return ::g_wnd_uid++; }
}; // Impl

Window_emitter::Window_emitter(Entity_editor_ctx& ctx): impl {new_unique<Impl>(ctx)} {}
Window_emitter::~Window_emitter() {}
void Window_emitter::imgui_exec() { impl->imgui_exec(); }

