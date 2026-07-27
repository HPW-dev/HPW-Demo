#include <cassert>
#include "anim-helper.hpp"
#include "game/core/anims.hpp"
#include "game/core/sprites.hpp"
#include "graphic/animation/anim.hpp"
#include "graphic/animation/frame.hpp"
#include "graphic/sprite/sprite.hpp"
#include "graphic/util/util-templ.hpp"

// создаёт спрайт только с белыми контурами по исходному спрайту
inline static Sprite extract_contour(cr<Sprite> src) {
  if (!src) {
    log_warning << "extract_contour src is empty";
    return {};
  }
  // расширение спрайта во все стороны на 1 пиксель (нужна только маска)
  Image ext_mask(src.X() + 2, src.Y() + 2, Pal8::mask_invisible);
  // вставить целевой спрайт в центр расширенного
  insert(ext_mask, src.mask(), Vec(1, 1));

  Sprite ret (ext_mask.X, ext_mask.Y);
  // определить контуры
  for (int y = 1; y < ext_mask.Y - 1; ++y)
  for (int x = 1; x < ext_mask.X - 1; ++x) {
    cauto mask_pix = ext_mask(x, y);
    cont_if (mask_pix == Pal8::mask_invisible);

    #define set_white(x0, y0) if (ext_mask(x0, y0) == Pal8::mask_invisible) { \
      ret.image().fast_set(x0, y0, Pal8::white, {}); \
      ret.mask().fast_set(x0, y0, Pal8::mask_visible, {}); \
    }
    set_white(x+1, y+0);
    set_white(x-1, y+0);
    set_white(x+0, y-1);
    set_white(x+0, y+1);
    #undef set_white
  } // for x/y

  ret.set_generated(true);
  ret.set_path(src.get_path() + ".extended");
  return ret;
}

cr<Shared<Anim>> make_light_mask(cr<Str> src, cr<Str> dst) {
  auto dst_anim = new_shared<Anim>();
  auto src_anim = hpw::anim_mgr->find_anim(src);
  assert(src_anim);

  *dst_anim = *src_anim;
  dst_anim->set_name(dst);
  // замена рисунка спрайта на обведёные контуры
  for (crauto frames: dst_anim->get_frames()) {
    for (crauto _direct: frames->get_directions()) {
      // допустимое преобразование:
      rauto direct = ccast<Direct&>(_direct);
      crauto sprite_for_contour = *direct.sprite.lock();

      direct.offset += -Vec(1, 1);
      direct.sprite = hpw::sprites.push(
        sprite_for_contour.get_path() + ".contour",
        new_shared<Sprite>(extract_contour(sprite_for_contour))
      );
    } // for directs
  } // for frames

  // закинуть новую анимацию в банк
  hpw::anim_mgr->add_anim(dst, dst_anim);
  return hpw::anim_mgr->find_anim(dst);
}
