#include <imgui.h>
#include "scene-entity-editor.hpp"
#include "yn.hpp"
#include "window.hpp"
#include "entity-window/wnd-ent-edit-menu.hpp"
#include "entity-window/wnd-ent-edit-opts.hpp"
#include "entity-window/window-emitter.hpp"
#include "entity-editor-util.hpp"
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
#include "game/core/messages.hpp"
#include "game/util/cmd/cmd.hpp"
#include "game/util/camera.hpp"
#include "game/util/sync.hpp"
#include "game/util/game-util.hpp"
#include "game/util/keybits.hpp"
#include "game/scene/scene-cmd.hpp"
#include "util/log.hpp"

struct Scene_entity_editor::Impl {
  Scene_entity_editor& m_master;
  Entity_editor_ctx m_ctx {}; // данные редактора распределяемые между окнами
  Vector<Unique<Window>> m_windows {};
  Vector<Unique<Window_emitter>> m_emitters {}; // окна под настройки спавнеров

  explicit inline Impl(Scene_entity_editor& master): m_master {master}
  { init(); }

  inline void update(const Delta_time dt) {
    // переключение в консоль
    if (is_pressed_once(hpw::keycode::console))
      hpw::scene_mgr->add(new_shared<Scene_cmd>());

    // удалить выключенные окна
    std::erase_if(m_emitters, [](CN<decltype(m_emitters)::value_type> window) {
      return !window->active();
    });

    if (!m_ctx.pause) {
      hpw::entity_mgr->update(dt);
      for (cnauto window: m_windows)
        window->update(dt);
      for (cnauto window: m_emitters)
        window->update(dt);
    }
  }

  inline void draw(Image& dst) const {
    draw_bg(dst);
    hpw::entity_mgr->draw(dst, graphic::camera->get_offset());
    for (cnauto window: m_windows)
      window->draw(dst);
    for (cnauto window: m_emitters)
      window->draw(dst);
    if (m_ctx.pause)
      draw_pause(dst);
  }

  inline void imgui_exec() {
    if (ImGui::IsKeyDown(ImGuiKey_Escape))
      exit();
    for (cnauto window: m_windows)
      window->imgui_exec();
    for (cnauto window: m_emitters)
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

  inline void save() { entity_editor_save(m_ctx); }

  inline void reload() {
    hpw_log("reloading entity editor...\n");
    init();
  }

  inline void draw_pause(Image& dst) const {
    // мигающая надпись пазуы
    return_if (graphic::frame_count % 30 > 15);
    graphic::font->draw(dst, get_screen_center(), U"П А У З А", &blend_diff);
  }

  inline void add_emitter() {
    auto wnd = new_unique<decltype(m_emitters)::value_type::element_type>(m_ctx);
    m_emitters.emplace_back(std::move(wnd));
  }

  inline void init() {
    detailed_log("entity editor init...\n");
    // graphic:
    graphic::cpu_safe = false;
    graphic::set_vsync(false);
    graphic::set_disable_frame_limit(!graphic::get_vsync());
    graphic::wait_frame = false;
    graphic::auto_frame_skip = false;
    graphic::blink_particles = false;
    graphic::disable_heat_distort_while_lag = false;
    graphic::light_quality = Light_quality::high;
    // resource:
    hpw::lazy_load_anim = true;
    hpw::lazy_load_sprite = true;
    load_resources();
    load_animations();
    // game:
    hpw::shmup_mode = true;
    init_shared(graphic::camera);
    init_unique(hpw::entity_mgr);
    // editor context:
    m_ctx = {};
    // показ текста на экране:
    init_unique(hpw::message_mgr);
    // log:
    #ifdef DETAILED_LOG
    std::stringstream log_txt;
    log_txt << "entity editor init info:\n";
    log_txt << "graphic:\n";
    log_txt << "  CPU safe mode = " << std::boolalpha << graphic::cpu_safe << '\n';
    log_txt << "  V-sync = " << graphic::get_vsync() << '\n';
    log_txt << "  wait frame for game tick = " << graphic::wait_frame << '\n';
    log_txt << "  blink particles = " << graphic::blink_particles << '\n';
    log_txt << "  auto frame skip = " << graphic::auto_frame_skip << '\n';
    log_txt << "resources:\n";
    log_txt << "  lazy anim loading = " << hpw::lazy_load_anim << '\n';
    log_txt << "  lazy sprite loading = " << hpw::lazy_load_sprite << '\n';
    log_txt << "  anim's = " << hpw::anim_mgr->anim_count() << '\n';
    log_txt << "  sprites = " << hpw::store_sprite->list().size() << '\n';
    log_txt << "game:\n";
    log_txt << "  entity loaders = " << hpw::entity_mgr->entity_loaders_sz() << '\n';
    log_txt << "  shmup mode = " << std::boolalpha << hpw::shmup_mode << '\n';
    detailed_log(log_txt.str());
    #endif

    // load entity editor data
    entity_editor_load(m_ctx);

    // windows
    m_emitters.clear();
    add_emitter();
    m_windows.clear();
    m_windows.push_back(new_unique<Wnd_ent_edit_menu>(m_master));
    m_windows.push_back(new_unique<Wnd_ent_edit_opts>(m_ctx));
    detailed_log("windows initialized\n");
  }
}; // Impl

Scene_entity_editor::Scene_entity_editor(): impl {new_unique<Impl>(*this)} {}
Scene_entity_editor::~Scene_entity_editor() {}
void Scene_entity_editor::update(const Delta_time dt) { impl->update(dt); }
void Scene_entity_editor::draw(Image& dst) const { impl->draw(dst); }
void Scene_entity_editor::imgui_exec() { impl->imgui_exec(); }
void Scene_entity_editor::exit() { impl->exit(); }
void Scene_entity_editor::pause() { impl->pause(); }
void Scene_entity_editor::save() { impl->save(); }
void Scene_entity_editor::reload() { impl->reload(); }
void Scene_entity_editor::add_emitter() { impl->add_emitter(); }
