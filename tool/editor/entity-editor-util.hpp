#pragma once

struct Entity_editor_ctx;
class Entity;

void entity_editor_save(const Entity_editor_ctx& ctx);
void entity_editor_load(Entity_editor_ctx& ctx);
void edit_flags(Entity& entity);
