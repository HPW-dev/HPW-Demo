#include <vector>
#include <algorithm>
#include "host-ogl.hpp"
#include "host/host-util.hpp"
#include "host/command.hpp"
#include "util/error.hpp"
#include "util/log.hpp"
#include "game/core/canvas.hpp"
#include "game/core/graphic.hpp"
#include "game/util/sync.hpp"
#include "game/util/keybits.hpp"
#include "game/util/resource-helper.hpp"
#include "game/util/game-archive.hpp"
#include "game/core/palette.hpp"
#include "graphic/image/color.hpp"

// --------- [!] ---------
// не переносить вверх!
#include "ogl.hpp"
// --------- [!] ---------

Host_ogl::Host_ogl(int argc, char *argv[])
: Protownd(argc, argv) {
  m_pixels = scast<decltype(m_pixels)>(graphic::canvas->data());
  init_archive(); // из архива понадобятся шейдеры
  #ifndef ECOMEM
    load_color_tables();
  #endif
}

void Host_ogl::ogl_resize(int w, int h) {
  return_if (w <= 0 || h <= 0);
  m_window_ctx.resize_by_mode(w, h, m_w, m_h);

  // настройка 2D координат и проекций OpenGL:
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, m_window_ctx.w, m_window_ctx.h, 0, 0, 1);
  glViewport(m_window_ctx.sx, m_window_ctx.sy, m_window_ctx.w, m_window_ctx.h);
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

  compie_vertex_shader();
  compie_fragment_shader();
  compile_program();

  init_screen_poly();
  ogl_resize(m_w, m_h);
} // ogl_init

void Host_ogl::init_palette_loader() {
  hpw::init_palette_from_archive = [this](cr<Str> fname) {
    graphic::current_palette_file = fname.empty() ? Str{graphic::DEFAULT_PALETTE_FILE} : fname;
    pal_tex_init();
  };
}

void Host_ogl::draw_game_frame() const {
  return_if (!graphic::enable_render);

  /* TODO пока не будут картинки по краям экрана, надо заливать чёрным,
  чтобы на линуксе не была эпилепсия */
  glClear(GL_COLOR_BUFFER_BIT);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_screen_tex); // текстура для graphic::canvas
  // перенос пикселей в текстуру
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_w, m_h, GL_RED,
    GL_UNSIGNED_BYTE, m_pixels);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_1D, m_pal_tex); // текстура палитры

  glBindVertexArray(m_vao); // растягивание текстуры
  glUseProgram(m_shader_prog); // применение шейдера
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // отрисовка текстуры на полигонах

  glUseProgram(0);
  glBindVertexArray(0); // unbind m_vao
  glBindTexture(GL_TEXTURE_1D, 0); // unbind m_pal_tex
  glBindTexture(GL_TEXTURE_2D, 0); // unbind m_screen_tex

#ifdef WINDOWS
  // stuttering fix
  if (graphic::get_vsync())
    glFinish();
#endif

  if ( !graphic::double_buffering)
    glFlush();
} // ogl_draw

// вызывается в конце инита наследника
void Host_ogl::ogl_post_init() {
  ogl_init();
  reshape(m_window_ctx.w, m_window_ctx.h);
}

void Host_ogl::init_screen_poly() {
  // квадрат из треугольников
  Vector<GLfloat> vertices{
    -1,  1, 0, 0,
    -1, -1, 0, 1,
     1,  1, 1, 0,
     1, -1, 1, 1
  };
  glGenBuffers(1, &m_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(),
    vertices.data(), GL_STATIC_DRAW);

  // Vertex layout
  glGenVertexArrays(1, &m_vao);
  glBindVertexArray(m_vao);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, {});

  glBindVertexArray(0); // unbind m_vao
  glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind m_vbo
} // init_screen_poly

void Host_ogl::compile_program() {
  m_shader_prog = glCreateProgram();
  glAttachShader(m_shader_prog, m_vert_shader);
  glAttachShader(m_shader_prog, m_frag_shader);
  glBindAttribLocation(m_shader_prog, 0, "pos");
  glLinkProgram(m_shader_prog);
  glUseProgram(m_shader_prog);
  glUniform1i(glGetUniformLocation(m_shader_prog, "main_tex"), 0);
  glUniform1i(glGetUniformLocation(m_shader_prog, "pal_tex"), 1);
  // check
  GLint result;
  int info_len;
  glGetProgramiv(m_shader_prog, GL_LINK_STATUS, &result);
  glGetProgramiv(m_shader_prog, GL_INFO_LOG_LENGTH, &info_len);
  if (info_len > 0) {
    Vector<char> info_msg(info_len + 1);
    glGetProgramInfoLog(m_shader_prog, info_len, {}, info_msg.data());
    hpw_log("Shader program info: " + Str(info_msg.begin(), info_msg.end()) + "\n", Log_stream::debug);
  }
} // compile_program

void Host_ogl::compie_vertex_shader() {
  auto source_file = load_res("resource/shader/default.vert");
  auto source_text = Str(source_file.data.begin(), source_file.data.end());
  auto source = scast<cp<GLchar>>(source_text.c_str());
  m_vert_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(m_vert_shader, 1, &source, 0);
  glCompileShader(m_vert_shader);
  // check
  GLint result;
  int info_len;
  glGetShaderiv(m_vert_shader, GL_COMPILE_STATUS, &result);
  glGetShaderiv(m_vert_shader, GL_INFO_LOG_LENGTH, &info_len);
  if (info_len > 0) {
    Vector<char> info_msg(info_len + 1);
    glGetShaderInfoLog(m_vert_shader, info_len, {}, info_msg.data());
    hpw_log( "Vertex shader info: " + Str(info_msg.begin(), info_msg.end()) );
  }
} // compie_vertex_shader

void Host_ogl::compie_fragment_shader() {
  auto source_file = load_res("resource/shader/default.frag");
  auto source_text = Str(source_file.data.begin(), source_file.data.end());
  auto source = scast<cp<GLchar>>(source_text.c_str());
  m_frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(m_frag_shader, 1, &source, 0);
  glCompileShader(m_frag_shader);
  // check
  GLint result;
  int info_len;
  glGetShaderiv(m_frag_shader, GL_COMPILE_STATUS, &result);
  glGetShaderiv(m_frag_shader, GL_INFO_LOG_LENGTH, &info_len);
  if (info_len > 0) {
    Vector<char> info_msg(info_len + 1);
    glGetShaderInfoLog(m_frag_shader, info_len, {}, info_msg.data());
    hpw_log( "Fragment shader info: " + Str(info_msg.begin(), info_msg.end()) );
  }
} // compie_fragment_shader

void Host_ogl::screen_tex_init() {
  check_max_gltex_sz();
  glGenTextures(1, &m_screen_tex);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_screen_tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_w, m_h, 0, GL_RGB, GL_FLOAT, {});
  glBindTexture(GL_TEXTURE_2D, 0);  // unbind m_screen_tex
} // scree_tex_init

void Host_ogl::pal_tex_init() {
  glGenTextures(1, &m_pal_tex);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_1D, m_pal_tex);
  // не делать линейным, иначе появятся розовые оттенки
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  // фиксит индекс палитры внутри шейдера
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  try {
    // загрузка палитры
    m_pal_rgb = load_ogl_palette(graphic::current_palette_file);
  } catch (...) {
    // если палитру не удастся получить, то сгенерировать её
    if (!graphic::current_palette_file.empty()) {
      hpw_log("не удалось загрузить файл палитры \"" +
        graphic::current_palette_file + "\"\n", Log_stream::warning);
      graphic::current_palette_file = {};
    }
    _gen_palette();
  }
  // в 1D текстуре хранятся триплеты палитры для шейдера
  glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 256, 0, GL_RGB, GL_FLOAT, m_pal_rgb.data());
  glBindTexture(GL_TEXTURE_1D, 0); // unbind m_pal_tex
} // pal_tex_init

void Host_ogl::check_max_gltex_sz() {
  auto max_canvas_sz = std::max(graphic::width, graphic::height);
  GLint avaliable_max_sz = -1;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &avaliable_max_sz);
  hpw_log("max texture size: " + n2s(avaliable_max_sz) + '\n', Log_stream::debug);
  // если эта ошибка выстрелит, то напиши разбивку на тайлы и их рендери
  iferror(avaliable_max_sz < max_canvas_sz,
    "GL_MAX_TEXTURE_SIZE < " << n2s(max_canvas_sz));
}
