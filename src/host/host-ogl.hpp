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
  uint m_screen_tex = 0; // OpenGL текстура экрана
  uint m_vao = 0, m_vbo = 0;
  uint m_shader_prog = 0, m_vert_shader = 0, m_frag_shader = 0;
  uint m_pal_tex = 0;
  CP<void> m_pixels {}; // данные для копирования в текстуру

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
