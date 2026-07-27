#include <cassert>
#include <array>
#include "microfont-mono.hpp"
#include "graphic/sprite/sprite.hpp"
#include "graphic/image/image-io.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/graphic-util.hpp"
#include "util/file/yaml.hpp"
#include "util/error.hpp"
#include "game/util/resource-helper.hpp"

struct Glyph {
  Veci offset {};
  Sprite spr {};
};

struct Microfont_mono::Impl {
  Microfont_mono& _master;
  std::array<Glyph, 128> _glyphs {};

  inline Impl(Microfont_mono& master, cr<Str> path)
  : _master {master} {
    // загрузить настройки шрифта
    cauto config_file = load_res(path);
    cauto config = Yaml(config_file);
    assert(config.check());

    // установить параметры из конфига
    cauto glyph_node = config["glyph"];
    assert(glyph_node.check());
    cauto w = glyph_node.get_int("w");
    cauto h = glyph_node.get_int("h");
    cauto space_w = glyph_node.get_int("space_w");
    cauto space_h = glyph_node.get_int("space_h");
    _master.set_w(w);
    _master.set_h(h);
    _master.set_space(Veci(space_w, space_h));
    assert(_master.w() > 0 && _master.w() < 1'000);
    assert(_master.h() > 0 && _master.h() < 1'000);
    assert(_master.space().x > -1 && _master.space().x < 100);
    assert(_master.space().y > -1 && _master.space().y < 100);

    // файл глифов
    cauto glyphs_image_path = config.get_str("glyphs_file");
    assert(!glyphs_image_path.empty());
    cauto glyphs_file = load_res(glyphs_image_path);
    Image glyphs_image;
    load(glyphs_image, glyphs_file.data);

    // нарезать глифы
    cauto grid_node = config["grid"];
    assert(grid_node.check());
    cauto grid_x = grid_node.get_int("w");
    cauto grid_y = grid_node.get_int("h");
    cauto grid_space = grid_node.get_int("space");
    assert(grid_x > 0 && grid_x < 1000);
    assert(grid_y > 0 && grid_y < 1000);
    assert(grid_space > -1 && grid_space < 100);

    std::size_t charcode = 0;
    cfor (x, grid_x)
    cfor (y, grid_y) {
      Glyph glyph;
      glyph.spr.init(w, h);
      glyph.spr.image().fill(Pal8::white);
      Veci pos(x * (w + grid_space), y * (h + grid_space));
      const Recti rect(pos, Veci(w, h));
      auto tile = cut(glyphs_image, rect, Image_get::NONE);
      glyph.spr.move_mask(std::move(tile));
      apply_invert(glyph.spr.mask());
      assert(glyph.spr.X() == w);
      assert(glyph.spr.Y() == h);
      _glyphs.at(charcode) = std::move(glyph);
      ++charcode;
    }

    // смещения глифов
    // TODO
  }

  inline void draw(Image& dst, const Veci pos, cr<utf32> txt, blend_pf bf, const int opt) const {
    assert(dst);
    return_if(txt.empty());
    int posx = pos.x;
    int posy = pos.y;
    
    for (uint limit = 0; auto ch: txt) {
      ++limit;

      // столько текста на экран не влезет
      break_if(limit >= 8'000);

      // пропуск строки
      if (ch == U'\n') {
        posy += _master.h() + _master.space().y;
        posx = pos.x;
        continue;
      }
      // \r возврат каретки
      if (ch == '\r') {
        posx = pos.x;
        continue;
      }

      cauto glyph = _glyphs.at(ch < _glyphs.size() ? ch : 127);
      insert(dst, glyph.spr, {posx + glyph.offset.x, posy + glyph.offset.y}, bf, opt);
      posx += _master.w() + _master.space().x;
    } // for text size
  }
};

Microfont_mono::Microfont_mono(cr<Str> path): _impl {new_unique<Impl>(*this, path)} {}
Microfont_mono::~Microfont_mono() {}
void Microfont_mono::draw(Image& dst, const Veci pos, cr<utf32> txt, blend_pf bf, const int opt) const
  { _impl->draw(dst, pos, txt, bf, opt); }
