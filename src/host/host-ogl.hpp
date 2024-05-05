#pragma once
#include "protownd.hpp"
#include "util/math/num-types.hpp"

// базовый класс для игрового хоста
class Host_ogl: public Protownd {
public:
  explicit Host_ogl(int argc, char *argv[]);
  ~Host_ogl() = default;

protected:
  void ogl_post_init(); // вызывается в конце инита наследника
  void ogl_draw(); // отрисовать кадр на текстуру
  void check_max_gltex_sz();
  virtual void draw(); // draw от main_f внутри execute

private:
  uint screen_tex_ = 0; // OpenGL текстура экрана
  uint vao_ = 0, vbo_ = 0;
  uint shader_prog_ = 0, vert_shader_ = 0, frag_shader_ = 0;
  uint pal_tex_ = 0;
  CP<void> pixels_ {}; // данные для копирования в текстуру

  // растягивает OpenGL полотно
  void ogl_resize(int w, int h) override;
  // настройка индексированной палитры
  void ogl_init();
  void init_screen_poly();
  void compile_program();
  void compie_vertex_shader();
  void compie_fragment_shader();
  void pal_tex_init();
  void screen_tex_init();
  void init_palette_loader();
}; // Host_ogl
