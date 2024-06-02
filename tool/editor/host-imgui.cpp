#include <cassert>
#include "editor.hpp"
#include "host-imgui.hpp"
#include "game/core/fonts.hpp"
#include "game/util/game-archive.hpp"
#include "graphic/font/unifont.hpp"
#include "util/error.hpp"
#include "util/file/file.hpp"
#include "util/file/archive.hpp"
#include "game/util/game-util.hpp"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

Host_imgui::Host_imgui(int argc_, char *argv_[])
: Host_glfw(argc_, argv_) {
  init_window();
} // c-tor

void Host_imgui::init_window() {
  Host_glfw::init_window();

  if (inited) {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
  }

// Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  //io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
// загрузка шрифта
  init_archive();
  auto font_file {hpw::archive->get_file("resource/font/unifont-13.0.06.ttf")};
  graphic::font = new_shared<Unifont>(font_file, 16);
  ImFontConfig font_cfg;
  // без этого шрифт нормально не грузится из памяти
  font_cfg.FontDataOwnedByAtlas = false;
  auto font = io.Fonts->AddFontFromMemoryTTF(
    font_file.data.data(), font_file.data.size(),
    16.0f, &font_cfg, io.Fonts->GetGlyphRangesCyrillic());
  check_p(font);
// Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init();
// Setup Dear ImGui style
  init_color_theme();

  inited = true;
} // init_window

Host_imgui::~Host_imgui() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void Host_imgui::draw() {
  Host_ogl::draw();
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  imgui_exec();
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  glPixelTransferi(GL_MAP_COLOR, 1);
} // draw

void Host_imgui::init_color_theme() {
  //ImGui::StyleColorsDark();
  ImGuiStyle& style = ImGui::GetStyle();
  style.IndentSpacing = 15;
  style.ScrollbarSize = 13;
  style.GrabMinSize = 11;
  style.WindowBorderSize = 0;
  style.FrameBorderSize = 0;
  style.WindowRounding = 0;
  style.ScrollbarRounding = 0;

  ImVec4* colors = ImGui::GetStyle().Colors;
  colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
  colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
  colors[ImGuiCol_WindowBg]               = ImVec4(0.09f, 0.09f, 0.09f, 0.93f);
  colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImGuiCol_PopupBg]                = ImVec4(0.09f, 0.09f, 0.09f, 0.94f);
  colors[ImGuiCol_Border]                 = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
  colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImGuiCol_FrameBg]                = ImVec4(0.28f, 0.28f, 0.28f, 0.54f);
  colors[ImGuiCol_FrameBgHovered]         = ImVec4(1.00f, 1.00f, 1.00f, 0.40f);
  colors[ImGuiCol_FrameBgActive]          = ImVec4(0.81f, 0.81f, 0.81f, 0.67f);
  colors[ImGuiCol_TitleBg]                = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
  colors[ImGuiCol_TitleBgActive]          = ImVec4(0.76f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
  colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
  colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
  colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
  colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
  colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
  colors[ImGuiCol_CheckMark]              = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
  colors[ImGuiCol_SliderGrab]             = ImVec4(0.53f, 0.53f, 0.53f, 1.00f);
  colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
  colors[ImGuiCol_Button]                 = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
  colors[ImGuiCol_ButtonHovered]          = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);
  colors[ImGuiCol_ButtonActive]           = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
  colors[ImGuiCol_Header]                 = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
  colors[ImGuiCol_HeaderHovered]          = ImVec4(0.57f, 0.57f, 0.57f, 0.80f);
  colors[ImGuiCol_HeaderActive]           = ImVec4(0.73f, 0.73f, 0.73f, 1.00f);
  colors[ImGuiCol_Separator]              = ImVec4(0.48f, 0.48f, 0.48f, 1.00f);
  colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
  colors[ImGuiCol_SeparatorActive]        = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
  colors[ImGuiCol_ResizeGrip]             = ImVec4(0.61f, 0.61f, 0.61f, 0.20f);
  colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
  colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
  colors[ImGuiCol_Tab]                    = ImVec4(0.34f, 0.34f, 0.34f, 0.86f);
  colors[ImGuiCol_TabHovered]             = ImVec4(0.45f, 0.45f, 0.45f, 0.80f);
  colors[ImGuiCol_TabActive]              = ImVec4(0.80f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_TabUnfocused]           = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
  colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
  colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
  colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
  colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
  colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
  colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
  colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
  colors[ImGuiCol_TableBorderLight]       = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
  colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
  colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
  colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
  colors[ImGuiCol_NavHighlight]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
  colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
  colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
} // init_color_theme
