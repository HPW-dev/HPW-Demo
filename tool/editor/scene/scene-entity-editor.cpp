#include "scene-entity-editor.hpp"
#include "graphic/image/image.hpp"

struct Scene_entity_editor::Impl {
  inline void update(const Delta_time dt) {}
  inline void draw(Image& dst) const {}
  inline void imgui_exec() {}
};

Scene_entity_editor::Scene_entity_editor(): impl {new_unique<Impl>()} {}
Scene_entity_editor::~Scene_entity_editor() {}
void Scene_entity_editor::update(const Delta_time dt) { impl->update(dt); }
void Scene_entity_editor::draw(Image& dst) const { impl->draw(dst); }
void Scene_entity_editor::imgui_exec() { impl->imgui_exec(); }
