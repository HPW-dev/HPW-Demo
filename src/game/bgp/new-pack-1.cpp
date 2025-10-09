#include "bgp.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/graphic-util.hpp"

namespace bgp {

// просто заливка
class Simple_color: public Bgp {
  Pal8 _color {};
public:
  inline Simple_color() { restart(); }
  inline void restart() { _color = rand_color_graphic(); }
  inline void draw(Image& dst) const { dst.fill(_color); }
};

// просто заливка
class Empty: public Bgp { public: inline void draw(Image& _) const {} };

BGP_REG_MAKER(Simple_color)
BGP_REG_MAKER(Empty)

} // bgp ns
