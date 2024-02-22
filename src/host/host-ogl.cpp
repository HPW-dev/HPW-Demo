#include <vector>
#include <algorithm>
#include "host-ogl.hpp"
#include "host-util.hpp"
#include "command.hpp"
#include "util/error.hpp"
#include "util/log.hpp"
#include "util/file/archive.hpp"
#include "game/game-canvas.hpp"
#include "game/game-graphic.hpp"
#include "game/game-sync.hpp"
#include "game/util/keybits.hpp"
#include "game/util/game-archive.hpp"
#include "game/util/game-palette.hpp"
#include "graphic/image/color.hpp"
#include "graphic/image/image.hpp"

// --------- [!] ---------
// не переносить вверх!
#include "ogl.hpp"
// --------- [!] ---------

Host_ogl::Host_ogl(int argc, char *argv[])
: Protownd(argc, argv) {
  pixels_ = scast<decltype(pixels_)>(graphic::canvas->data());
  init_archive(); // из архива понадобятся шейдеры
  load_color_tables();
}

void Host_ogl::draw() { ogl_draw(); }

void Host_ogl::ogl_resize(int w, int h) {
  if ( !w || !h)
    return;
  window_ctx_.resize_by_mode(w, h, w_, h_);
// настройка 2D координат и проекций OpenGL:
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, window_ctx_.w, window_ctx_.h, 0, 0, 1);
  glViewport(window_ctx_.sx, window_ctx_.sy, window_ctx_.w, window_ctx_.h);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
} // ogl_resize

void Host_ogl::ogl_init() {
  // отключаю ненужное для оптимизации
  glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);
  glDisable(GL_LIGHT0);
  glDisable(GL_LIGHTING);
  glDisable(GL_COLOR_MATERIAL);
  glDisable(GL_BLEND);
  glDisable(GL_MULTISAMPLE); // отключение сглаживания
  glShadeModel(GL_FLAT); // не гладкая заливка

  glClearColor(0,0,0,0);
  pal_tex_init();
  init_palette_loader();
  screen_tex_init();

  check_p(hpw::archive);
  compie_vertex_shader();
  compie_fragment_shader();
  compile_program();

  init_screen_poly();
  ogl_resize(w_, h_);
} // ogl_init

void Host_ogl::init_palette_loader() {
  hpw::init_palette_from_archive = [this](const Str& fname) {
    graphic::current_palette_file = fname;
    pal_tex_init();
  };
}

/// отрисовать кадр на текстуру
void Host_ogl::ogl_draw() {
  /* TODO пока не будут картинки по краям экрана, надо заливать чёрным,
  чтобы на линуксе не была эпилепсия */
  glClear(GL_COLOR_BUFFER_BIT);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, screen_tex_); // текстура для graphic::canvas
  // перенос пикселей в текстуру
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w_, h_, GL_RED,
    GL_UNSIGNED_BYTE, pixels_);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_1D, pal_tex_); // текстура палитры

  glBindVertexArray(vao_); // растягивание текстуры
  glUseProgram(shader_prog_); // применение шейдера
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // отрисовка текстуры на полигонах

  glUseProgram(0);
  glBindVertexArray(0); // unbind vao_
  glBindTexture(GL_TEXTURE_1D, 0); // unbind pal_tex_
  glBindTexture(GL_TEXTURE_2D, 0); // unbind screen_tex_

#ifdef WINDOWS
  // stuttering fix
  if (graphic::get_vsync())
    glFinish();
#endif

  if ( !graphic::double_buffering)
    glFlush();
} // ogl_draw

/// вызывается в конце инита наследника
void Host_ogl::ogl_post_init() {
  ogl_init();
  reshape(window_ctx_.w, window_ctx_.h);
}

void Host_ogl::init_screen_poly() {
  // квадрат из треугольников
  Vector<float> vertices{
    -1,  1, 0, 0,
    -1, -1, 0, 1,
     1,  1, 1, 0,
     1, -1, 1, 1
  };
  glGenBuffers(1, &vbo_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(),
    vertices.data(), GL_STATIC_DRAW);

  // Vertex layout
  glGenVertexArrays(1, &vao_);
  glBindVertexArray(vao_);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, {});

  glBindVertexArray(0); // unbind vao_
  glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind vbo_
} // init_screen_poly

void Host_ogl::compile_program() {
  shader_prog_ = glCreateProgram();
  glAttachShader(shader_prog_, vert_shader_);
  glAttachShader(shader_prog_, frag_shader_);
  glBindAttribLocation(shader_prog_, 0, "pos");
  glLinkProgram(shader_prog_);
  glUseProgram(shader_prog_);
  glUniform1i(glGetUniformLocation(shader_prog_, "main_tex"), 0);
  glUniform1i(glGetUniformLocation(shader_prog_, "pal_tex"), 1);
  // check
  GLint result;
  int info_len;
  glGetProgramiv(shader_prog_, GL_LINK_STATUS, &result);
  glGetProgramiv(shader_prog_, GL_INFO_LOG_LENGTH, &info_len);
  if (info_len > 0) {
    Vector<char> info_msg(info_len + 1);
    glGetProgramInfoLog(shader_prog_, info_len, {}, info_msg.data());
    detailed_log("Shader program info: "
      << Str(info_msg.begin(), info_msg.end()) << "\n");
  }
} // compile_program

void Host_ogl::compie_vertex_shader() {
  auto source_file = hpw::archive->get_file("resource/shader/default.vert");
  auto source_text = Str(source_file.data.begin(), source_file.data.end());
  auto source = scast<CP<GLchar>>(source_text.c_str());
  vert_shader_ = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vert_shader_, 1, &source, 0);
  glCompileShader(vert_shader_);
  // check
  GLint result;
  int info_len;
  glGetShaderiv(vert_shader_, GL_COMPILE_STATUS, &result);
  glGetShaderiv(vert_shader_, GL_INFO_LOG_LENGTH, &info_len);
  if (info_len > 0) {
    Vector<char> info_msg(info_len + 1);
    glGetShaderInfoLog(vert_shader_, info_len, {}, info_msg.data());
    hpw_log("Vertex shader info: " << Str(info_msg.begin(), info_msg.end()) );
  }
} // compie_vertex_shader

void Host_ogl::compie_fragment_shader() {
  auto source_file = hpw::archive->get_file("resource/shader/default.frag");
  auto source_text = Str(source_file.data.begin(), source_file.data.end());
  auto source = scast<CP<GLchar>>(source_text.c_str());
  frag_shader_ = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(frag_shader_, 1, &source, 0);
  glCompileShader(frag_shader_);
  // check
  GLint result;
  int info_len;
  glGetShaderiv(frag_shader_, GL_COMPILE_STATUS, &result);
  glGetShaderiv(frag_shader_, GL_INFO_LOG_LENGTH, &info_len);
  if (info_len > 0) {
    Vector<char> info_msg(info_len + 1);
    glGetShaderInfoLog(frag_shader_, info_len, {}, info_msg.data());
    hpw_log("Fragment shader info: " << Str(info_msg.begin(), info_msg.end()) );
  }
} // compie_fragment_shader

void Host_ogl::screen_tex_init() {
  check_max_gltex_sz();
  glGenTextures(1, &screen_tex_);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, screen_tex_);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w_, h_, 0, GL_RGB, GL_FLOAT, {});
  glBindTexture(GL_TEXTURE_2D, 0);  // unbind screen_tex_
} // scree_tex_init

void Host_ogl::pal_tex_init() {
  glGenTextures(1, &pal_tex_);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_1D, pal_tex_);
  // не делать линейным, иначе появятся розовые оттенки
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  // фиксит индекс палитры внутри шейдера
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  try {
    // загрузка палитры
    pal_rgb = load_ogl_palette(graphic::current_palette_file);
  } catch (...) {
    // если палитру не удастся получить, то сгенерировать её
    if (!graphic::current_palette_file.empty()) {
      hpw_log("не удалось загрузить файл палитры \"" <<
        graphic::current_palette_file << "\"\n");
      graphic::current_palette_file = {};
    }
    _gen_palette();
  }
  // в 1D текстуре хранятся триплеты палитры для шейдера
  glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 256, 0, GL_RGB, GL_FLOAT, pal_rgb.data());
  glBindTexture(GL_TEXTURE_1D, 0); // unbind pal_tex_
} // pal_tex_init

void Host_ogl::check_max_gltex_sz() {
  auto max_canvas_sz = std::max(graphic::width, graphic::height);
  GLint avaliable_max_sz = -1;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &avaliable_max_sz);
  detailed_log("max texture size: " << avaliable_max_sz << '\n');
  // если эта ошибка выстрелит, то напиши разбивку на тайлы и их рендери
  iferror(avaliable_max_sz < max_canvas_sz,
    "GL_MAX_TEXTURE_SIZE < " << n2s(max_canvas_sz));
}
