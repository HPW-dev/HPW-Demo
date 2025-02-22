#include <cassert>
#include <ranges>
#include <queue>
#include <functional>
#include "scene-mgr.hpp"
#include "scene.hpp"

struct Scene_mgr::Impl {
  using Scenes = Vector<Shared<Scene>>;
  using Actions = std::queue<std::function<void ()>>;
  Status _status {};
  Scenes _scenes {};
  Actions _actions {};

  inline void add(cr<Shared<Scene>> scene) {
    assert(scene);

    _actions.emplace([this, scene]{
      _scenes.emplace_back(std::move(scene));
      _status.next_scene = true;
      _status.empty = false;
    });
  }

  inline bool update(Delta_time dt) {
    return_if (_scenes.empty() && _actions.empty(), false);

    _status.next_scene = false;
    _status.came_back = false;

    while (!_actions.empty()) {
      _actions.front()();
      _actions.pop();
    }
    _status.empty = _scenes.empty();

    cauto cur = current();
    return_if (!cur, false);
      
    cur->update(dt);
    return true;
  }

  inline void draw(Image& dst) const {
    return_if (_scenes.empty());
    cauto cur = current();
    assert(cur);
    cur->draw(dst);
  }

  inline void back(uint count) {
    cfor (i, count)
      _actions.emplace ( [this] {
        if (!_scenes.empty()) {
          _scenes.pop_back();
          _status.came_back = true;
        } else {
          _status.empty = true;
        }
      } );
  }

  inline Shared<Scene> current() const {
    return_if (!_scenes.empty(), _scenes.front());
    return {};
  }

  inline Strs names() const {
    Strs ret;

    for (crauto scene: _scenes)
      ret.emplace_back(scene->name());

    return ret;
  }

  inline cr<Status> status() const { return _status; }

  inline void clear() {
    _status.came_back = false;
    _status.next_scene = false;
    _status.empty = true;
    _actions = {};
    _scenes.clear();
  }
}; // Impl

Scene_mgr::Scene_mgr(): _impl {new_unique<Impl>()} {}
Scene_mgr::~Scene_mgr() {}
void Scene_mgr::add(cr<Shared<Scene>> scene) { _impl->add(scene); }
bool Scene_mgr::update(Delta_time dt) { return _impl->update(dt); }
void Scene_mgr::draw(Image& dst) const { _impl->draw(dst); }
void Scene_mgr::back(uint count) { _impl->back(count); }
Shared<Scene> Scene_mgr::current() const { return _impl->current(); }
Strs Scene_mgr::names() const { return _impl->names(); }
cr<Scene_mgr::Status> Scene_mgr::status() const { return _impl->status(); }
void Scene_mgr::clear() { _impl->clear(); }
