#include "pch.hpp"
#include "tilemap.hpp"
#include "engine/graphic/image/image.hpp"
#include "engine/graphic/sprite/sprite.hpp"
#include "engine/graphic/sprite/sprite-io.hpp"
#include "engine/graphic/util/graphic-util.hpp"
#include "engine/graphic/util/util-templ.hpp"
#include "game/core/sprites.hpp"
#include "game/util/game-archive.hpp"
#include "game/util/resource-helper.hpp"

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
  inline Impl(cr<Str> fname, const bool is_archive = true) {
    if (is_archive) {
      load_from_archive(fname);
    } else {
      load_from_resources(fname);
    }
  }

  inline void load(cr<nlohmann::json> config, cr<Str> config_path, std::optional<cp<Archive>> archive = {}) {
    auto tilemap_node = config["tilemap"];

    m_source_config = config_path;
    m_original_w = tilemap_node["original_w"].get<int>();
    m_original_h = tilemap_node["original_h"].get<int>();
    m_tile_w = tilemap_node["tile_w"].get<int>();;
    m_tile_h = tilemap_node["tile_h"].get<int>();;

    auto tiles_node = tilemap_node["tiles"];
    for (crauto tile_name: root_tags(tiles_node)) {
      auto cur_tile_node = tiles_node[tile_name];
      auto fname = cur_tile_node["file"].get<Str>();
      auto offset_v = cur_tile_node["offset"].get<Vector<int>>();
      Vec offset(offset_v.at(0), offset_v.at(1));

      Shared<Sprite> sprite;
      Str sprite_fname;
      if (archive) {
        // искать спрайты в корне архива:
        auto sprite_file = archive.value()->get_file(fname);
        init_shared<Sprite>(sprite);
        ::load(*sprite, sprite_file);
        // указать что это из архива загружено
        sprite_fname = archive.value()->get_path() + "/" + fname;
        sprite->set_path(sprite_fname);
        // закинуть в банк
        sprite = hpw::sprites.move(sprite_fname, std::move(sprite));
      } else {
        // спрайты должны лежать там же, где и конфиг:
        auto config_fname = config_path;
        conv_sep(config_fname);
        auto sprite_dir = get_filedir(config_fname) + "/";
        sprite_fname = sprite_dir + fname;
        conv_sep_for_archive(sprite_fname);
        sprite = hpw::sprites.find(sprite_fname);
      }

      iferror(!sprite, "sprite \"" << sprite_fname << "\" not founded in sprite_store");
      m_tiles.emplace_back( std::move( Tile{
        .sprite = sprite,
        .offset = offset
      } ) );
    } // for tile names
  } // load

  // случай, когда все тайлы заархивированы
  inline void load_from_archive(cr<Str> fname) {
    auto tiles_archived = load_res(fname);
    const Archive archive(std::move(tiles_archived));
    cauto config_fname = "tilemap.json";
    cauto config_file = archive.get_file(config_fname);
    
    try {
      cauto config = nlohmann::json::parse(config_file.data);
      load(config, config_fname, &archive);
    } catch (cr<nlohmann::json::parse_error> e) {
      const Str msg = Str("error while parsing JSON data from \"") + config_fname + "\":\n"
        + "  " + e.what();
      error(msg);
    }
  }

  inline void load_from_resources(cr<Str> fname) {
    load(json_from_res(fname), fname);
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
Tilemap::Tilemap(cr<Str> fname, const bool is_archive): impl{new_unique<Impl>(fname, is_archive)} {}
Tilemap::~Tilemap() {}
void Tilemap::draw(const Vec pos, Image& dst, blend_pf bf, int optional) const { impl->draw(pos, dst, bf, optional); }
int Tilemap::get_original_w() const { return impl->get_original_w(); }
int Tilemap::get_original_h() const { return impl->get_original_h(); }
int Tilemap::get_tile_w() const { return impl->get_tile_w(); }
int Tilemap::get_tile_h() const { return impl->get_tile_h(); }
