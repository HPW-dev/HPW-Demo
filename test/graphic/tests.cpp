#include <cmath>
#include <iostream>
#include "graphic-test.hpp"
#include "util/error.hpp"
#include "util/log.hpp"
#include "graphic/util/rotation.hpp"
#include "graphic/util/graphic-util.hpp"
#include "graphic/util/util-templ.hpp"
#include "graphic/sprite/sprite.hpp"
#include "game/game-canvas.hpp"

inline void color_check() {
  std::cout << "color check" << std::endl;
  // размер должен быть 1
  hpw_assert(sizeof(Pal8) == 1);
  // начальное значение = 0
  Pal8 dummy;
  hpw_assert(dummy == 0);
  // инкремент 0 = 1
  ++dummy;
  hpw_assert(dummy == 1);
  // декремент 1 = 0
  --dummy;
  hpw_assert(dummy == 0);
  // декремент 0 = 0, нет переполнения для палитрового цвета
  --dummy;
  hpw_assert(dummy == 0);
  // инкремент максимального цвета = он же, без переполнения
  dummy = Pal8::white;
  ++dummy;
  hpw_assert(dummy == Pal8::white);
  // красный-чёрный при декременте не становится серым оттенком
  dummy = Pal8::red_black;
  --dummy;
  hpw_assert(dummy == Pal8::red_black);
  // красный-яркий при инкременте не выходит за диапазон красных оттенков
  dummy = Pal8::red_end;
  ++dummy;
  hpw_assert(dummy == Pal8::red_end);
  // сложение серого не доходит до красных оттенков
  hpw_assert( !(Pal8::get_gray(Pal8::gray) + Pal8::get_gray(Pal8::gray)).is_red());
  // сложение красных не выходит за предел красных оттенков
  hpw_assert( (Pal8::get_red(Pal8::red) + Pal8::get_red(Pal8::red)).is_red());
  // при сложении красного с любым цветом, цвет конвертируется в красный
  hpw_assert( (Pal8::get_gray(Pal8::gray) + Pal8::get_red(Pal8::red)).is_red());
  // проверка флага выключеного красного
  hpw_assert( !Pal8::from_real(0.5, false).is_red());
  // проверка флага включёного красного
  hpw_assert(Pal8::from_real(0.5, true).is_red());
  // максимальный белый преобразуется к 1 при val -> real
  hpw_assert(Pal8(Pal8::white).to_real() == 1);
  // серый-чёрный преобразуется к 0 при val -> real
  hpw_assert(Pal8(Pal8::black).to_real() == 0);
  // максимальный красный преобразуется к 1 при val -> real
  hpw_assert(Pal8(Pal8::red).to_real() == 1);
  // максимальный красный-чёрный преобразуется к 0 при val -> real
  hpw_assert(Pal8(Pal8::red_black).to_real() == 0);
  // 0 - 1 = 0, нет переполнения для палитрового цвета
  hpw_assert(blend_sub_safe(Pal8(1), Pal8(0)) == 0);
  // 0 + 1 = 1
  hpw_assert(blend_add_safe(Pal8(1), Pal8(0)) == 1);
  // при смешивании чёрного с белым при прозрачности 158/255 должен получиться target_158
  static const int target_158 = std::ceil(real(96.0/255.0) * Pal8::gray_end);
  hpw_assert(blend_158(Pal8::black, Pal8::white) == target_158);
} // color_check

inline void image_check() {
  std::cout << "image check" << std::endl;
  // в пустой картинке всё по нулям
  Image dummy;
  hpw_assert(dummy.size == 0);
  hpw_assert(dummy.X == 0);
  hpw_assert(dummy.Y == 0);
  hpw_assert( !dummy);
  // проверка размеров при копировании
  dummy = Image(5, 3);
  hpw_assert(dummy);
  hpw_assert(dummy.X == 5);
  hpw_assert(dummy.Y == 3);
  hpw_assert(dummy.size == 5*3);
  // проверка неправильных инитов
  dummy.init(999, 0);
  hpw_assert( !dummy);
  dummy.init(0, 999);
  hpw_assert( !dummy);
  dummy.init(9'999'999, 9'999'999);
  hpw_assert( !dummy);
  // тест очистки через пустое копирование
  dummy = Image{};
  hpw_assert(dummy.X == 0);
  hpw_assert(dummy.Y == 0);
  hpw_assert(dummy.size == 0);
  hpw_assert( !dummy);
  // тест очистки через free
  dummy.free();
  hpw_assert(dummy.X == 0);
  hpw_assert(dummy.Y == 0);
  hpw_assert(dummy.size == 0);
  hpw_assert( !dummy);
  // проверка размеров при ините
  dummy.init(3, 5);
  hpw_assert(dummy);
  hpw_assert(dummy.X == 3);
  hpw_assert(dummy.Y == 5);
  hpw_assert(dummy.size == 3*5);
  // проверка заливки при ините
  dummy.init(5, 5, Pal8::red);
  for (cnauto pix: dummy)
    hpw_assert(pix == Pal8::red);
  // проверка заливки отдельно
  dummy.fill(Pal8::white);
  for (cnauto pix: dummy)
    hpw_assert(pix == Pal8::white);
  // ресайз до того же размера
  dummy.init(5, 5);
  auto* old_data = dummy.data();
  dummy.assign_resize(5, 5);
  auto* new_data = dummy.data();
  hpw_assert(old_data == new_data);
  // резайз уменьшающий
  dummy.assign_resize(3, 3);
  new_data = dummy.data();
  hpw_assert(old_data == new_data);
  // ресайз до первоначального размера
  dummy.assign_resize(5, 5);
  new_data = dummy.data();
  hpw_assert(old_data == new_data);
  // ресайз на увеличение, буффер должен выделиться
  dummy.assign_resize(7, 7);
  new_data = dummy.data();
  hpw_assert(old_data != new_data);
  // за пределами картинки должен быть Pal8{}
  dummy.init(5, 5, Pal8::red);
  hpw_assert(dummy.get( 3,  3, Image_get::NONE) == Pal8::red);
  hpw_assert(dummy.get(-1, -1, Image_get::NONE) == Pal8::none);
  hpw_assert(dummy.get( 6, -1, Image_get::NONE) == Pal8::none);
  hpw_assert(dummy.get(-1,  6, Image_get::NONE) == Pal8::none);
  hpw_assert(dummy.get( 6,  6, Image_get::NONE) == Pal8::none);
  // при зеркальном режиме copy должна быть копия пикселя с краю
  cfor (x, dummy.X)
    dummy(x, 0) = Pal8::white;
  hpw_assert(dummy.get( 3,  3, Image_get::COPY) == Pal8::red);
  hpw_assert(dummy.get( 3, -1, Image_get::COPY) == Pal8::white);
  hpw_assert(dummy.get( 3,  6, Image_get::COPY) == Pal8::red);
  hpw_assert(dummy.get(-1,  3, Image_get::COPY) == Pal8::red);
  hpw_assert(dummy.get( 3,  6, Image_get::COPY) == Pal8::red);
  // никаких установок пикселей за пределы быть не должно
  dummy.set(-1,  3, Pal8::gray, {});
  dummy.set( 6,  3, Pal8::gray, {});
  dummy.set( 3, -1, Pal8::gray, {});
  dummy.set( 3,  6, Pal8::gray, {});
  dummy.set( 3,  3, Pal8::gray, {});
  hpw_assert(dummy.get(-1,  3) != Pal8::gray);
  hpw_assert(dummy.get( 6,  3) != Pal8::gray);
  hpw_assert(dummy.get( 3, -1) != Pal8::gray);
  hpw_assert(dummy.get( 3,  6) != Pal8::gray);
  hpw_assert(dummy.get( 3,  3) == Pal8::gray);
  // основное полотно уже должно быть создано
  hpw_assert(graphic::canvas); // ptr allocated
  hpw_assert(*graphic::canvas); // check Image
  hpw_assert(graphic::canvas->X == graphic::width);
  hpw_assert(graphic::canvas->Y == graphic::height);
} // image_check

inline void sprite_check() {
  std::cout << "sprite check" << std::endl;
  // пустой инит
  Sprite dummy;
  hpw_assert( !dummy);
  dummy = Sprite{};
  hpw_assert( !dummy);
  // тест размеров
  dummy = Sprite(3, 5);
  hpw_assert(dummy);
  hpw_assert(dummy.X() == 3);
  hpw_assert(dummy.Y() == 5);
  // тест размера маски и картинки
  auto* mask = dummy.get_mask();
  auto* image = dummy.get_image();
  hpw_assert(mask);
  hpw_assert(mask->X == 3);
  hpw_assert(mask->Y == 5);
  hpw_assert(image);
  hpw_assert(image->X == 3);
  hpw_assert(image->Y == 5);
  // тест вставки. По середине спрайта красный, снизу справа белый
  dummy = Sprite(3, 3);
  dummy.get_image()->set(1, 1, Pal8::red, {});
  dummy.get_mask()->set(1, 1, Pal8::mask_visible, {});
  dummy.get_image()->set(2, 2, Pal8::white, {});
  dummy.get_mask()->set(2, 2, Pal8::mask_visible, {});
  Image for_insert(5, 5, Pal8::gray);
  insert(for_insert, dummy, {1,1});
  hpw_assert(for_insert.get(0,0) == Pal8::gray);
  hpw_assert(for_insert.get(4,4) == Pal8::gray);
  hpw_assert(for_insert.get(2,2) == Pal8::red);
  hpw_assert(for_insert.get(3,3) == Pal8::white);
  hpw_assert(for_insert.get(3,4) == Pal8::gray);
  hpw_assert(for_insert.get(2,3) == Pal8::gray);
} // sprite_check

inline void util_check() {
  std::cout << "util check" << std::endl;
  // тест правильного поворота. На картинке точка, смотришь за ней
  {
    Image dummy(3, 5, Pal8::white);
    // 0 deg
    dummy(0, 0) = Pal8::red;
    hpw_assert(dummy(0, 0) == Pal8::red);
    hpw_assert(dummy(dummy.X-1, 0) == Pal8::white);
    hpw_assert(dummy(0, dummy.Y-1) == Pal8::white);
    hpw_assert(dummy(dummy.X-1, dummy.Y-1) == Pal8::white);
    // 90 deg
    auto dummy90 = rotate90(dummy);
    hpw_assert(dummy90.X == dummy.Y);
    hpw_assert(dummy90.Y == dummy.X);
    hpw_assert(dummy90(0, 0) == Pal8::white);
    hpw_assert(dummy90(dummy90.X-1, 0) == Pal8::red);
    hpw_assert(dummy90(0, dummy90.Y-1) == Pal8::white);
    hpw_assert(dummy90(dummy90.X-1, dummy90.Y-1) == Pal8::white);
    // 180 deg
    auto dummy180 = rotate90(dummy, 2);
    hpw_assert(dummy180.X == dummy.X);
    hpw_assert(dummy180.Y == dummy.Y);
    hpw_assert(dummy180(0, 0) == Pal8::white);
    hpw_assert(dummy180(dummy180.X-1, 0) == Pal8::white);
    hpw_assert(dummy180(0, dummy180.Y-1) == Pal8::white);
    hpw_assert(dummy180(dummy180.X-1, dummy180.Y-1) == Pal8::red);
    // 270 deg
    auto dummy270 = rotate90(dummy, 3);
    hpw_assert(dummy270.X == dummy.Y);
    hpw_assert(dummy270.Y == dummy.X);
    hpw_assert(dummy270(0, 0) == Pal8::white);
    hpw_assert(dummy270(dummy270.X-1, 0) == Pal8::white);
    hpw_assert(dummy270(0, dummy270.Y-1) == Pal8::red);
    hpw_assert(dummy270(dummy270.X-1, dummy270.Y-1) == Pal8::white);
  } // rotate 90 test
} // util_check

void graphic_tests() {
  try {
    color_check();
    image_check();
    sprite_check();
    util_check();
  } catch (CN<hpw::Error> ex) {
    std::cerr << ex.what() << std::endl;
    std::terminate();
  }
}
