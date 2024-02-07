#include <iostream>
#include <cstdlib>
#include <cmath>
#include <string>
#include "graphic/image/image-io.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/graphic-util.hpp"
#include "util/math/rect.hpp"
#include "util/file/yaml.hpp"
#include "util/str.hpp"
#include "util/str-util.hpp"
#include "util/path.hpp"

int main(int argc, char* argv[]) {
  if (argc < 4) {
    std::cout << "Warning: need more args!" << std::endl;
    std::cout << "Usage: tilemap-gen.exe \"_image_for_cut_\" _size_x_ _size_y_" << std::endl;
    std::cout << "(Output in same dir)" << std::endl;
    return EXIT_SUCCESS;
  }
  
  Str fname = argv[1];
  int tile_x = std::stoi(argv[2]);
  int tile_y = std::stoi(argv[3]);

  Image loaded;
  load(loaded, fname);

  std::cout << "Image name: \"" << loaded.get_path() << "\"" << std::endl;
  std::cout << "Image width: " << loaded.X << std::endl;
  std::cout << "Image height: " << loaded.Y << std::endl;
  std::cout << "tile width: " << tile_x << std::endl;
  std::cout << "tile height: " << tile_y << std::endl;

  // не нарезать, если не надо
  if (loaded.X <= tile_x && loaded.Y <= tile_y) {
    std::cerr << "размер тайла больше, чем вся картинка, нарезка не требуется" << std::endl;
    return EXIT_FAILURE;
  }

  Yaml config;
  auto tilemap_node = config.make_node("tilemap");
  tilemap_node.set_str("original_name", loaded.get_path());
  tilemap_node.set_int("original_w", loaded.X);
  tilemap_node.set_int("original_h", loaded.Y);
  tilemap_node.set_int("tile_w", tile_x);
  tilemap_node.set_int("tile_h", tile_y);
  auto tiles_node = tilemap_node.make_node("tiles");

  // нарезать на кусочки и засейвить их по отдельности
  cfor (y, scast<int>(std::ceil(loaded.Y / scast(double, tile_y))) )
  cfor (x, scast<int>(std::ceil(loaded.X / scast(double, tile_x))) ) {
    Rect rect(x * tile_x, y * tile_y, tile_x, tile_y);
    // не вырезать больше, чем исходник
    if (rect.pos.x + tile_x >= loaded.X)
      rect.size.x = loaded.X - rect.pos.x;
    if (rect.pos.y + tile_y >= loaded.Y)
      rect.size.y = loaded.Y - rect.pos.y;
    // вырезание
    auto tile = cut(loaded, rect, Image_get::NONE);

    // сохранение тайла в файл
    auto tile_name = "x" + n2s(x) + "y" + n2s(y);
    Str tile_dir = get_filedir(fname);
    auto tile_fname = "tile-" + tile_name + ".png";
    auto tile_fname_with_dir = tile_dir + SEPARATOR + tile_fname;
    save(tile, tile_fname_with_dir);

    // сохранение тайла в конфиг
    auto tile_node = tiles_node.make_node("tile_" + tile_name);
    tile_node.set_str("file", tile_fname);
    tile_node.set_v_int("offset", {scast<int>(rect.pos.x), scast<int>(rect.pos.y)});
  }

  config.save(fname + "-tilemap.yml");
  return EXIT_SUCCESS;
}
