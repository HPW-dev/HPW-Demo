#include "entity-editor-ctx.hpp"
#include "game/core/common.hpp"
#include "game/core/entities.hpp"
#include "game/util/game-util.hpp"
#include "game/entity/util/entity-util.hpp"
#include "util/log.hpp"
#include "util/error.hpp"
#include "util/str-util.hpp"
#include "util/macro.hpp"

namespace {
Str g_entities_yml_path = "config/entities.yml";
}

void entity_editor_save(CN<Entity_editor_ctx> ctx) {
  auto entities_yml_path = hpw::cur_dir + ::g_entities_yml_path;
  conv_sep(entities_yml_path);
  ctx.entities_yml.save(entities_yml_path);
  hpw_log("entity editor data saved\n");
}

void entity_editor_load(Entity_editor_ctx& ctx) {
  auto entities_yml_path = hpw::cur_dir + ::g_entities_yml_path;
  conv_sep(entities_yml_path);

  try {
    // доступ к файлу с настройками объектов
    ctx.entities_yml = Yaml(entities_yml_path);
    // загрузить объекты
    init_unique(hpw::entity_mgr);
    hpw::entity_mgr->register_types();
    // выбрать коллайдер
    set_default_collider();
    hpw_log("entity editor data loaded\n");
  } catch (...) {
    error("неизвестная ошибка при чтении файла \"" +
      entities_yml_path + "\". Возможно что файла нет и его надо достать из data.zip");
  }
}
