#include "cutscene-0.hpp"
#include "game/util/keybits.hpp"
#include "game/core/scenes.hpp"
#include "graphic/image/image.hpp"
#include "graphic/util/graphic-util.hpp"
#include "graphic/util/util-templ.hpp"
#include "game/scene/scene-mgr.hpp"
#include "util/math/vec.hpp"
#include "util/vector-types.hpp"
#include "util/math/circle.hpp"
#include "util/math/random.hpp"

// рисует космос в рамочке
struct Space {
  // слои с разной яркостью
  struct Layers {
    Pal8 m_brightness {}; // яркость фона

    // планетарные системы
    struct System {
      Circle star {};
      struct Planet {
        real r {}; // радиус планеты
        real deg {}; // по какому углу от солнца расположена
        real range {}; // на какое растояние от солнца удалена планета
      };
      Vector<Planet> planets {};

      inline explicit System(uint w, uint h) {
        // сделать звезду
        star.offset = Vec(rnd(0, w), rnd(0, h));
        star.r = rndr(4, 10);

        // наделать планет
        cfor (_, rnd(0, 12))  {
          planets.emplace_back(Planet{
            .r = rndr(0.5, star.r),
            .deg = rand_degree_graphic(),
            .range = rndr(star.r+1, 40)
          });
        }
      } // System c-tor

      inline void draw(Image& dst, const Vec offset, Pal8 brightness) const {
        draw_circle_filled(dst, star.offset, star.r, brightness);
        // нарисовать кольца и планеты
        for (crauto planet: planets) {
          draw_circle(dst, star.offset, planet.range, brightness);
          Vec pos = deg_to_vec(planet.deg) * planet.range;
          draw_circle_filled(dst, star.offset + pos, planet.r, brightness);
        }
      }
    }; // System

    Vector<System> systems {};

    explicit inline Layers(Pal8 brightness, uint w, uint h)
    : m_brightness {brightness}
    {
      // нагенерить планетарных систем
      cfor (_, rnd(2, 9))
        systems.emplace_back(w, h);
    }

    inline void draw(Image& dst) const {
      for (crauto system: systems)
        system.draw(dst, {}, m_brightness); // TODO offset
      // соединить системы прямой
      cfor (i, systems.size()-1) {
        auto a = systems[i].star.offset; 
        auto b = systems[i+1].star.offset;
        draw_line(dst, a, b, m_brightness); // TODO offset
      }
    }
  }; // Layers

  mutable Image frame {};
  Vector<Layers> layers {};

  Space() = default;

  inline explicit Space(uint w, uint h)
  : frame(w, h)
  {
    layers.emplace_back( Pal8::from_real(0.17), w, h );
    layers.emplace_back( Pal8::from_real(0.25), w, h );
    layers.emplace_back( Pal8::from_real(0.5), w, h );
    layers.emplace_back( Pal8::from_real(1), w, h );
  }

  inline void update(const Delta_time dt) {
    // motion
  }

  inline void draw(Image& dst, const Vec pos) const {
    frame.fill(Pal8::black);
    for (rauto layer: layers)
      layer.draw(frame);
    draw_rect(frame, Rect(0,0, frame.X-1, frame.Y-1), Pal8::white);

    insert(dst, frame, pos);
  }
}; // Space

struct Cutscene_0::Impl {
  Space space {};

  inline Impl(): space(192, 256) {}

  inline void update(const Delta_time dt) {
    if (is_pressed_once(hpw::keycode::escape))
      hpw::scene_mgr->back();

    space.update(dt);
  }

  inline void draw(Image& dst) const {  
    dst.fill(Pal8::black);
    space.draw(dst, Vec(256, 48));
  }
};

Cutscene_0::Cutscene_0(): impl{new_unique<Impl>()} {}
Cutscene_0::~Cutscene_0() {}
void Cutscene_0::update(const Delta_time dt) { impl->update(dt); }
void Cutscene_0::draw(Image& dst) const { impl->draw(dst); }
