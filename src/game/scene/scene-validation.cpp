#include <cassert>
#include <deque>
#include <functional>
#include "hash_sha256/hash_sha256.h"
#include "scene-validation.hpp"
#include "graphic/font/font.hpp"
#include "graphic/image/image.hpp"
#include "game/scene/scene-manager.hpp"
#include "game/scene/scene-main-menu.hpp"
#include "game/game-common.hpp"
#include "game/game-font.hpp"
#include "util/file/file.hpp"
#include "util/platform.hpp"
#include "util/str-util.hpp"

struct Job {
  using update_ft = std::function<bool (double)>;
  using draw_ft = std::function<void (Image&)>;
  update_ft m_update_fn {};
  draw_ft m_draw_fn {};
  bool m_updated {false};
  bool m_upd_cond {false};
  mutable bool m_drawed {false};

  inline explicit Job(update_ft update_fn, draw_ft draw_fn={})
  : m_update_fn {update_fn}
  , m_draw_fn {draw_fn}
  {}

  inline void update(double dt) {
    m_updated = true;
    if (!m_draw_fn)
      m_drawed = true;
    if (m_update_fn && m_upd_cond == false)
      m_upd_cond = m_update_fn(dt);
    else
      m_upd_cond = true;
  } // update

  inline void draw(Image& dst) const {
    return_if (!m_updated);
    if (m_draw_fn && !m_drawed)
      m_draw_fn(dst);
    m_drawed = true;
  }

  inline bool is_complete() const
  { return m_drawed && m_updated && m_upd_cond; }

}; // Job

Str calc_sum(CP<void> data, std::size_t sz) {
  hash_sha256 hash;
  hash.sha256_init();
  hash.sha256_update(scast<CP<uint8_t>>(data), sz);
  auto hash_ret = hash.sha256_final();
        
  std::stringstream ss;
  for (auto val: hash_ret)
    ss << std::hex << int(val);
  return str_toupper(ss.str());
}

struct Scene_validation::Impl {
  std::deque<Job> jobs {};

  inline ~Impl() = default;

  inline Impl() {
    // показать чё за меню на экране
    jobs.emplace_back( Job(
      [](double dt) { return true; },
      [](Image& dst) { graphic::font->draw(dst, Vec(15, 10), U"MEM CHECK..."); }
    ) );
    // EXE sum
    jobs.emplace_back( Job(
      [](double dt) {
        #ifdef WINDOWS
          Str path = hpw::cur_dir + "HPW.exe";
        #else
          Str path = hpw::cur_dir + "HPW";
        #endif
        auto mem = mem_from_file(path);
        hpw::exe_sha256 = calc_sum( scast<CP<void>>(mem.data()), mem.size() );
        return true;
      },
      [](Image& dst)
      { graphic::font->draw(dst, Vec(15, 30), U"EXE SHA256: " + sconv<utf32>(hpw::exe_sha256)); }
    ) );
    // DATA sum
    jobs.emplace_back( Job(
      [](double dt) {
        Str path = hpw::cur_dir + "data.zip";
        auto mem = mem_from_file(path);
        hpw::data_sha256 = calc_sum( scast<CP<void>>(mem.data()), mem.size() );
        return true;
      },
      [](Image& dst)
      { graphic::font->draw(dst, Vec(15, 50), U"DATA SHA256: " + sconv<utf32>(hpw::data_sha256)); }
    ) );
    // ждать нажатия клавиши
    jobs.emplace_back( Job(
      [](double dt) { return hpw::any_key_pressed; },
      [](Image& dst) { graphic::font->draw(dst, Vec(15, 70), U"PRESS ANY KEY"); }
    ) );
    // переход в следующую сцену
    jobs.emplace_back( Job(
      [](double dt) {
        hpw::scene_mgr->add(new_shared<Scene_main_menu>());
        return true;
      }
    ) );
  } // Impl c-tor

  inline void update(double dt) {
    if (jobs.empty()) { // если нет заданий, выйти из сцены
      hpw::scene_mgr->back();
    } else {
      nauto cur_job = jobs.front();

      if (cur_job.is_complete()) { // если работа выполнена, удилть её из списка
        jobs.pop_front();
        return;
      }

      cur_job.update(dt);
    }
  } // update
  
  inline void draw(Image& dst) const {
    if (!jobs.empty())
      jobs.front().draw(dst);
  } // draw

}; /// Impl

Scene_validation::Scene_validation(): impl {new_unique<Impl>()} {}
Scene_validation::~Scene_validation() {}
void Scene_validation::update(double dt) { impl->update(dt); }
void Scene_validation::draw(Image& dst) const { impl->draw(dst); }
