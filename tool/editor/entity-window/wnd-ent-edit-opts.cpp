#include <imgui.h>
#include "wnd-ent-edit-opts.hpp"

struct Wnd_ent_edit_opts::Impl {
  inline void imgui_exec() {

  }
}; // Impl

Wnd_ent_edit_opts::Wnd_ent_edit_opts(): impl {new_unique<Impl>()} {}
Wnd_ent_edit_opts::~Wnd_ent_edit_opts() {}
void Wnd_ent_edit_opts::imgui_exec() { impl->imgui_exec(); }
