#include <algorithm>
#include "resize.hpp"
#include "util/math/num-types.hpp"

void Resize_ctx::resize_by_mode(
int &new_w, int &new_h, int old_w, int old_h) {
  switch (this->mode) {
    default:
    case Resize_mode::one_to_one: {
      if (new_w < old_w || new_h < old_h) {
        new_w = old_w;
        new_h = old_h;
      }
      this->h = old_h * (new_h / old_h);
      this->w = this->h * (real(old_w) / old_h);
      this->sx = std::max(0, new_w / 2 - this->w / 2);
      this->sy = std::max(0, new_h / 2 - this->h / 2);
      this->ex = new_w / 2 + this->w / 2;
      this->ey = new_h / 2 + this->h / 2;
      break;
    }
    case Resize_mode::by_width: {
      this->w = new_w;
      this->h = this->w * (real(old_h) / old_w);
      this->sx = std::max(0, new_w / 2 - this->w / 2);
      this->sy = std::max(0, new_h / 2 - this->h / 2);
      this->ex = new_w / 2 + this->w / 2;
      this->ey = new_h / 2 + this->h / 2;
      break;
    }
    case Resize_mode::by_height: {
      this->h = new_h;
      this->w = this->h * (real(old_w) / old_h);
      this->sx = std::max(0, new_w / 2 - this->w / 2);
      this->sy = std::max(0, new_h / 2 - this->h / 2);
      this->ex = new_w / 2 + this->w / 2;
      this->ey = new_h / 2 + this->h / 2;
      break;
    }
    case Resize_mode::full: {
      this->w = this->ex = new_w;
      this->h = this->ey = new_h;
      this->sx = this->sy = 0;
      break;
    }
  } // switch resize mode
} // resize_by_mode

bool Resize_ctx::is_bad() const
{ return ex <= sx || ey <= sy || w <= 0 || h <= 0; }
