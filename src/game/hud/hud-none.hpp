#include "hud.hpp"

// не рисует никакой интерфейс
class Hud_none final: public Hud {
  nocopy(Hud_none);

public:
  Hud_none() = default;
  inline void draw(Image& dst) const {}
  inline void update(const Delta_time dt) {}
};
