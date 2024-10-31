#include "rect.hpp"

bool intersect(const Rect a, const Rect b) noexcept {
  cauto x1 = a.pos.x;
  cauto y1 = a.pos.y;
  cauto x2 = b.pos.x;
  cauto y2 = b.pos.y;
  cauto w1 = a.size.x;
  cauto h1 = a.size.y;
  cauto w2 = b.size.x;
  cauto h2 = b.size.y;
  return (x1  + w1 >= x2) && // r1 right edge past r2 left
         (x1 <= x2  + w2) && // r1 left edge past r2 right
         (y1  + h1 >= y2) && // r1 top edge past r2 bottom
         (y1 <= y2  + h2);   // r1 bottom edge past r2 top
}
