#include <cassert>
#include <ranges>
#include <functional>
#include <stack>
#include "scene-mgr.hpp"
#include "scene.hpp"

struct Scene_mgr::Impl {
  Status _status {};

  inline void add(cr<Shared<Scene>> scene) {
    // TODO
  }

  inline bool update(Delta_time dt) {
    return {}; // TODO
  }

  inline void draw(Image& dst) const {
    // TODO
  }

  inline void back(uint count) {
    // TODO
  }

  inline Shared<Scene> current() const {
    return {}; // TODO
  }

  inline Strs list() const {
    return {}; // TODO
  }

  inline cr<Status> status() const { return _status; }
}; // Impl

Scene_mgr::Scene_mgr(): _impl {new_unique<Impl>()} {}
Scene_mgr::~Scene_mgr() {}
void Scene_mgr::add(cr<Shared<Scene>> scene) { _impl->add(scene); }
bool Scene_mgr::update(Delta_time dt) { return _impl->update(dt); }
void Scene_mgr::draw(Image& dst) const { _impl->draw(dst); }
void Scene_mgr::back(uint count) { _impl->back(count); }
Shared<Scene> Scene_mgr::current() const { return _impl->current(); }
Strs Scene_mgr::list() const { return _impl->list(); }
cr<Scene_mgr::Status> Scene_mgr::status() const { return _impl->status(); }
