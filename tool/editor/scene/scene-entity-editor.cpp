#include <cassert>
#include <imgui.h>
#include "scene-entity-editor.hpp"
#include "yn.hpp"
#include "window.hpp"
#include "entity-window/wnd-ent-edit-menu.hpp"
#include "entity-window/wnd-ent-edit-opts.hpp"
#include "entity-editor-ctx.hpp"
#include "graphic/image/image.hpp"
#include "game/core/scenes.hpp"
#include "game/core/canvas.hpp"
#include "game/core/graphic.hpp"
#include "game/core/anims.hpp"
#include "game/core/sprites.hpp"
#include "game/core/entities.hpp"
#include "game/core/common.hpp"
#include "game/core/fonts.hpp"
#include "game/util/camera.hpp"
#include "game/util/sync.hpp"
#include "game/util/game-util.hpp"
#include "util/log.hpp"

struct Scene_entity_editor::Impl {
  Scene_entity_editor* m_master {};
  Entity_editor_ctx m_ctx {}; // данные редактора распределяемые между окнами
  Vector<Unique<Window>> m_windows {};

  explicit inline Impl(Scene_entity_editor* master)
  : m_master {master} {
    assert(m_master);
    m_windows.push_back(new_unique<Wnd_ent_edit_menu>(m_master));
    m_windows.push_back(new_unique<Wnd_ent_edit_opts>(m_ctx));
    init();
  }

  inline void update(const Delta_time dt) {
    if (!m_ctx.pause) {
      hpw::entity_mgr->update(dt);
    }

    for (cnauto window: m_windows)
      window->update(dt);
  }

  inline void draw(Image& dst) const {
    draw_bg(dst);
    hpw::entity_mgr->draw(dst, graphic::camera->get_offset());
    for (cnauto window: m_windows)
      window->draw(dst);
    if (m_ctx.pause)
      draw_pause(dst);
  }

  inline void imgui_exec() {
    if (ImGui::IsKeyDown(ImGuiKey_Escape))
      exit();
    for (cnauto window: m_windows)
      window->imgui_exec();
  }

  inline void exit() {
    hpw::scene_mgr->add( new_shared<Yes_no_scene>(
      u8"Выйти из редактора?", []{ hpw::scene_mgr->back(); } ) );
  }

  inline void draw_bg(Image& dst) const {
    cauto bg_color = Pal8::from_real(m_ctx.bg_color, m_ctx.red_bg);
    graphic::canvas->fill(bg_color);
  }

  inline void pause() { m_ctx.pause = !m_ctx.pause; }

  inline void save() {
    // TODO
    hpw_log("need impl");
  }

  inline void reload() {
    // TODO
    hpw_log("need impl");
  }

  inline void draw_pause(Image& dst) const {
    // мигающая надпись пазуы
    return_if (graphic::frame_count % 30 > 15);
    graphic::font->draw(dst, get_screen_center(), U"П А У З А", &blend_diff);
  }

  inline void init() {
    // graphic:
    graphic::cpu_safe = false;
    graphic::set_vsync(true);
    // resource:
    hpw::lazy_load_anim = true;
    hpw::lazy_load_sprite = true;
    load_resources();
    load_animations();
    // game:
    hpw::shmup_mode = true;
    graphic::camera = new_shared<Camera>();
    hpw::entity_mgr = new_unique<Entity_mgr>();
    // log:
    std::stringstream log_txt;
    log_txt << "entity editor init...\n";
    log_txt << "graphic:\n";
    log_txt << "  CPU safe mode = " << std::boolalpha << graphic::cpu_safe << '\n';
    log_txt << "  V-sync = " << graphic::get_vsync() << '\n';
    log_txt << "resources:\n";
    log_txt << "  lazy anim loading = " << hpw::lazy_load_anim << '\n';
    log_txt << "  lazy sprite loading = " << hpw::lazy_load_sprite << '\n';
    log_txt << "  anim's = " << hpw::anim_mgr->anim_count() << '\n';
    log_txt << "  sprites = " << hpw::store_sprite->list().size() << '\n';
    log_txt << "game:\n";
    log_txt << "  entity loaders = " << hpw::entity_mgr->entity_loaders_sz() << '\n';
    log_txt << "  shmup mode = " << std::boolalpha << hpw::shmup_mode << '\n';
    hpw_log(log_txt.str());
  }
}; // Impl

Scene_entity_editor::Scene_entity_editor(): impl {new_unique<Impl>(this)} {}
Scene_entity_editor::~Scene_entity_editor() {}
void Scene_entity_editor::update(const Delta_time dt) { impl->update(dt); }
void Scene_entity_editor::draw(Image& dst) const { impl->draw(dst); }
void Scene_entity_editor::imgui_exec() { impl->imgui_exec(); }
void Scene_entity_editor::exit() { impl->exit(); }
void Scene_entity_editor::pause() { impl->pause(); }
void Scene_entity_editor::save() { impl->save(); }
void Scene_entity_editor::reload() { impl->reload(); }
