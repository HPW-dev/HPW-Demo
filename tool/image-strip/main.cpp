#include <vector>
#include <iostream>
#include <sstream>
#include <cstdint>
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#include "util/vector-types.hpp"

using color_t = uint32_t; // RRGGBBAA

color_t s2color(const std::string& x) {
  color_t ret;
  std::stringstream ss;
  ss << std::hex << x;
  ss >> ret;
  return ret;
}

color_t* get_pixel(stbi_uc* data, int x, int y, int X, int comp) {
  auto ptr = reinterpret_cast<color_t*>(data);
  return &ptr[y * X + x];
}

int main(int argc, char *argv[]) {
  using namespace std;
// парс аргументов
  cout << "usage: in out color(RRGGBBAA) delete_color(1-0)\n";
  if (argc < 5) {
    cerr << "need more args: see usage\n";
    return EXIT_FAILURE;
  }
  string input(argv[1]);
  string output(argv[2]);
  color_t color(s2color(argv[3]));
  bool delete_color(stoi(argv[4]));
// прочитать картинку
  int X, Y, comp;
  stbi_uc *decoded = stbi_load(input.c_str(), &X, &Y, &comp, STBI_rgb_alpha);
  if ( !decoded)
    cerr << "image data is not decoded\n";
  cout << "input: " << X << "x" << Y << "\n";
// найти края для обрезания
  int l=X, r=0, u=Y, d=0; // стороны по которые будет вырезание
  for (int y = 0; y < Y; ++y)
  for (int x = 0; x < X; ++x) {
    auto pix = get_pixel(decoded, x, y, X, STBI_rgb_alpha);
    if (*pix != color) {
      l = std::min(l, x);
      r = std::max(r, x);
      u = std::min(u, y);
      d = std::max(d, y);
    }
  }
  d += 1;
  r += 1;
  cout << "strip by " << l << ":" << r << ":" << u << ":" << d
    << " (l:r:u:d)\n";
// записать обрезанный вариант с удалением цвета
  int new_X = r - l;
  int new_Y = d - u;
  auto encoded = Vector<stbi_uc>(new_X * new_Y * STBI_rgb_alpha);
  for (int y = 0; y < new_Y; ++y)
  for (int x = 0; x < new_X; ++x) {
    auto dst = get_pixel(encoded.data(), x, y, new_X, STBI_rgb_alpha);
    auto src = get_pixel(decoded, l+x, u+y, X, STBI_rgb_alpha);
    if (color == *src && delete_color)
      *dst = 0x000000FFu; // прозрачность
    else
      *dst = *src;
  }
  stbi_write_png(output.c_str(), new_X, new_Y, STBI_rgb_alpha, encoded.data(), 0);
// free
  stbi_image_free(decoded);
} //main
