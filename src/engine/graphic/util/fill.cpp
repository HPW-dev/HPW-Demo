#include "pch.hpp"
#include "fill.hpp"
#include "engine/graphic/image/image.hpp"

// заливка в 4 стороны:
static void floodfill4(Image& dst, Vec pos, Pal8 filler, Pal8 bg_color) {
  // Если цвет точки уже равен целевому, ничего делать не нужно
  ret_if (filler == bg_color);

  // Создать пустой Стек для хранения пар координат (x, y)
  std::stack<Vec> positions;
  positions.push(pos);

  while (!positions.empty()) {
    cauto p = positions.top();
    positions.pop();
    auto x = p.x;
    auto y = p.y;
        
    /* Смещаемся максимально влево по текущей строке, 
    пока цвет пикселей равен bg_color */
    while (x >= 0 && dst.get(x, y) == bg_color)
      --x;
    ++x; // Возвращаемся на первый валидный пиксель слева
        
    bool up = false;
    bool down = false;

    // Идем вправо, закрашивая пиксели и проверяя соседние строки
    while (x < dst.X && dst.get(x, y) == bg_color) {
      dst.set(x, y, filler);
            
      // Проверка строки ВЫШЕ (y - 1)
      if (y > 0) {
        if (dst.get(x, y - 1) == bg_color) {
          if (up == false) {
            positions.push(Vec(x, y - 1));
            up = true;
          }
        } else {
          up = false;
        }
      }
            
      // Проверка строки НИЖЕ (y + 1)
      if (y < dst.Y - 1) {
        if (dst.get(x, y + 1) == bg_color) {
          if (down == false) {
            positions.push(Vec(x, y + 1));
            down = true;
          }
        } else {
          down = false;
        }
      }
                    
      ++x;
    }
  } // while stack not empty
}

void fill(Image& dst, Vec pos, Pal8 filler) {
  assert(dst);
  floodfill4(dst, pos, filler, dst.get(pos.x, pos.y));
}