#include <cassert>
#include "scene-manager.hpp"
#include "scene.hpp"

bool Scene_mgr::update(double dt) {
  status = {}; // флаги сбрасываются каждый апдейт
  lazy_scene_update();

  if (auto current = get_current(); current)
    current->update(dt);
  else // если текущая сцена пустая, то спустится назад до новой
    back();
  
  return scene_stack.empty() != true; // true если сцены ещё есть
} // update

void Scene_mgr::draw(Image& dst) const {
  if (auto current = get_current(); current)
    current->draw(dst);
}

void Scene_mgr::add(CN<Shared<Scene>> scene) {
  assert(scene);
  jobs.emplace_back([this, scene]{ _add(scene); });
}

void Scene_mgr::back() {
  jobs.emplace_back([this]{ _back(); });
}

void Scene_mgr::_add(CN<Shared<Scene>> scene) {
  scene_stack.emplace(scene);
  status.next_scene = true;
}

void Scene_mgr::_back() {
  if ( !scene_stack.empty()) {
    scene_stack.pop();
    status.came_back = true;
  }
} // _back

Scene* Scene_mgr::get_current() const {
  if (scene_stack.empty()) {
    status.empty = true;
    return nullptr;
  }
  return scene_stack.top().get();
}

void Scene_mgr::lazy_scene_update() {
  // выполнить все задания в очереди и удалить учередь
  for (cnauto job: jobs)
    job();
  jobs.clear();
}
