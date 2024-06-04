#pragma once
#include "game/core/sprites.hpp"
#include "game/core/difficulty.hpp"
#include "util/unicode.hpp"
#include "util/str.hpp"
#include "util/mem-types.hpp"
#include "util/math/num-types.hpp"

class Image;
class Anim;
class Sprite;
class Yaml;
class Locale;
struct Circle;
struct Polygon;
struct Vec;

void load_animations();
void load_resources();
// безопасное получение локализованной строки
CN<utf32> get_locale_str(CN<Str> key);
// сделать круг полностью перекрывающий все полигоны
Circle cover_polygons(CN<Vector<Polygon>> polygons);
Vec get_screen_center();

/** создаёт осветлённую обводку для контуров спрайта
* @details анимация сама сохранится в банк
* @param src с какой анимации взять спрайты
* @param dst как назвать новую анимацию
* @return ссыль получившуюся анимацию */
CN<Shared<Anim>> make_light_mask(CN<Str> src, CN<Str> dst);

Str get_random_replay_name();
[[nodiscard]] Vec get_rand_pos_safe(const real sx, const real sy, const real ex, const real ey);
[[nodiscard]] Vec get_rand_pos_graphic(const real sx, const real sy, const real ex, const real ey);
// рисует нажатые игровые клавиши
void draw_controls(Image& dst);
// вычисляет контрольные суммы по экзешнику и data.zip
void init_validation_info();
void init_scene_mgr();
// перевести названия уровней сложности
utf32 difficulty_to_str(const Difficulty difficulty);
// сохраняет все текстуры в папку в виде атласной текстуры
void save_all_sprites(CN<Str> save_dir, const int MX=512, const int MY=512);
// размер всех спрайтов в банке
std::size_t sizeof_all_sprites();
// загрузить звуки в звуковую систему
void load_sounds();
