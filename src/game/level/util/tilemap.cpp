#include <omp.h>
#include <cassert>
#include <utility>
#include "tilemap.hpp"
#include "util/file/yaml.hpp"
#include "util/math/vec.hpp"
#include "graphic/image/image.hpp"
#include "graphic/sprite/sprite.hpp"
#include "graphic/util/graphic-util.hpp"
#include "graphic/util/util-templ.hpp"
#include "game/core/sprites.hpp"
#include "game/util/game-archive.hpp"
#include "game/util/resource-helper.hpp"
#include "util/path.hpp"

struct Tile {
  Weak<Sprite> sprite {}; // текстура с банка
  Vec offset {};
};

struct Tilemap::Impl {

  Str m_source_config {}; // откуда тайл был загружен
  Vector<Tile> m_tiles {}; 
  int m_original_w {};
  int m_original_h {};
  int m_tile_w {};
  int m_tile_h {};

  inline Impl() = default;
  inline Impl(cr<Str> fname) { load_from_archive(fname); }

  inline void load(cr<Yaml> config) {
    auto tilemap_node = config["tilemap"];

    m_source_config = tilemap_node.get_path();
    m_original_w = tilemap_node.get_int("original_w");
    m_original_h = tilemap_node.get_int("original_h");
    m_tile_w = tilemap_node.get_int("tile_w");
    m_tile_h = tilemap_node.get_int("tile_h");

    auto tiles_node = tilemap_node["tiles"];
    for (crauto tile_name: tiles_node.root_tags()) {
      auto cur_tile_node = tiles_node[tile_name];
      auto fname = cur_tile_node.get_str("file");
      auto offset_v = cur_tile_node.get_v_int("offset");
      Vec offset(offset_v.at(0), offset_v.at(1));

      // спрайты должны лежать там же, где и конфиг
      auto config_fname = config.get_path();
      conv_sep(config_fname);
      auto sprite_dir = get_filedir(config_fname) + "/";
      auto sprite_fname = sprite_dir + fname;
      conv_sep_for_archive(sprite_fname);

      auto sprite = hpw::sprites.find(sprite_fname);
      iferror(!sprite, "sprite \"" << sprite_fname << "\" not founded in sprite_store");
      m_tiles.emplace_back( std::move( Tile{
        .sprite = sprite,
        .offset = offset
      } ) );
    } // for tile names
  } // load

  inline void load_from_archive(cr<Str> fname) {
    auto file_data = load_res(fname);
    Yaml config(file_data);
    load(config);
  }

  inline void draw(const Vec pos, Image& dst, blend_pf bf=&blend_past, int optional=0) const {
    assert(!m_tiles.empty());

    #pragma omp parallel for schedule(dynamic)
    for (crauto tile: m_tiles) {
      iferror(tile.sprite.expired(), "tile.sprite bad ptr");
      // TODO не рисовать за экраном
      insert(dst, *tile.sprite.lock(), pos + tile.offset, bf, optional);
    }
  }

  inline int get_original_w() const { return m_original_w; }
  inline int get_original_h() const { return m_original_h; }
  inline int get_tile_w() const { return m_tile_w; }
  inline int get_tile_h() const { return m_tile_h; }

}; // Impl

Tilemap::Tilemap(Tilemap&& other): impl{std::move(other.impl)} {}
Tilemap::Tilemap(): impl{new_unique<Impl>()} {}
Tilemap::Tilemap(cr<Str> fname): impl{new_unique<Impl>(fname)} {}
Tilemap::~Tilemap() {}
void Tilemap::load(cr<Yaml> config) { impl->load(config); }
void Tilemap::load_from_archive(cr<Str> fname) { impl->load_from_archive(fname); }
void Tilemap::draw(const Vec pos, Image& dst, blend_pf bf, int optional) const { impl->draw(pos, dst, bf, optional); }
int Tilemap::get_original_w() const { return impl->get_original_w(); }
int Tilemap::get_original_h() const { return impl->get_original_h(); }
int Tilemap::get_tile_w() const { return impl->get_tile_w(); }
int Tilemap::get_tile_h() const { return impl->get_tile_h(); }
