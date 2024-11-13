#include <ranges>
#include <cassert>
#include "scene-locale.hpp"
#include "game/core/scenes.hpp"
#include "game/core/common.hpp"
#include "game/util/game-util.hpp"
#include "game/util/keybits.hpp"
#include "game/menu/table-menu.hpp"
#include "game/menu/item/table-row-item.hpp"
#include "game/util/game-archive.hpp"
#include "util/file/yaml.hpp"

struct Locale_info {
  Str path {};
  utf32 name {};
  utf32 author {};
};

struct Scene_locale_select::Impl {
  Unique<Table_menu> _menu {};
  // промежуточное представление о файлах локализации
  Vector<Locale_info> _locale_infos {};

  inline Impl() {
    load_locale_info();
    init_menu();
  }

  inline void load_locale_info() {
    assert(hpw::archive);
    cauto files = hpw::archive->get_all_names();
    cauto locale_dir = Str("resource/locale/");
    cauto filter = [locale_dir](cr<Str> fname) {
      bool dir_equ_path = fname.find(locale_dir) == 0;
      bool path_not_dir = fname.size() > locale_dir.size();
      return dir_equ_path && path_not_dir;
    };

    // пойтись по файлам локализаций
    for (crauto path: files | std::views::filter(filter)) {
      try {
        // загрузить инфу об авторе и название локализации
        cauto locale_yml_file = hpw::archive->get_file(path);
        cauto locale_yml = Yaml(locale_yml_file);
        cauto info_node = locale_yml["info"];

        // засейвить в промежуточное представление
        Locale_info info;
        info.path = path;
        info.name = utf8_to_32(info_node.get_str("name"));
        info.author = utf8_to_32(info_node.get_str("author"));
        _locale_infos.emplace_back(std::move(info));
      } catch (...) {
        hpw_log("проблемы при загрузке локализации в \"" + path + "\"\n", Log_stream::warning);
      }
    }
  } // load_locale_info

  inline void init_menu() {
    cauto title = U"Язык • Language • 言語 • 언어";
    cauto row_height = 25u;
    cauto elems_empty_txt = get_locale_str("common.empty");
    utf32 translation_str = U"Перевод • Translation • 翻訳";
    utf32 author_str = U"Автор • Author • 著者";
    // переопределить строки хедера, если язык уже выбран
    if (!hpw::first_start) {
      translation_str = get_locale_str("scene.locale_select.translation");
      author_str = get_locale_str("common.author");
    }
    Table_menu::Rows rows {{translation_str, 330}, {author_str, 0}};
    Menu_items items;
    for (crauto info: _locale_infos) {
      auto item = new_shared<Menu_item_table_row>(
        [info]{
          load_locale(info.path);
          assert(hpw::scene_mgr);
          hpw::scene_mgr->back();
        },
        Menu_item_table_row::Content_getters {
          [info]{ return info.name; },
          [info]{ return info.author; },
        }
      );

      items.emplace_back(std::move(item));
    }
    init_unique(_menu, title, rows, row_height, items, elems_empty_txt);
  }

  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape))
      hpw::scene_mgr->back();

    _menu->update(dt);
  }

  inline void draw(Image& dst) const {
    _menu->draw(dst);
  }
}; // impl

Scene_locale_select::Scene_locale_select(): impl {new_unique<Impl>()} {}
Scene_locale_select::~Scene_locale_select() {}
void Scene_locale_select::update(const Delta_time dt) { impl->update(dt); }
void Scene_locale_select::draw(Image& dst) const { impl->draw(dst); }

