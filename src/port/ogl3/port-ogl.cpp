#include "pch.hpp"
#include "port-ogl.hpp"
#include "port/port-util.hpp"
#include "port/command.hpp"
#include "game/core/canvas.hpp"
#include "game/core/palette.hpp"
#include "game/core/graphic.hpp"
#include "game/util/sync.hpp"
#include "game/input/keys.hpp"
#include "game/util/resource-helper.hpp"
#include "engine/graphic/image/color.hpp"

#ifndef DISABLE_CONFIG
#include "game/util/game-archive.hpp"
#endif

// --------- [!] ---------
// не переносить вверх!
#include "ogl.hpp"
// --------- [!] ---------

Host_ogl::Host_ogl(int argc, char *argv[])
: Protownd(argc, argv) {
  m_pixels = scast<decltype(m_pixels)>(graphic::canvas->data());
  #ifndef DISABLE_CONFIG
    init_archive(); // из архива понадобятся шейдеры
  #endif
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
  glDisable(GL_CULL_FACE); // без отсечения обратных граней полигона
  glDisable(GL_ALPHA_TEST); // Альфа канал не чекать
  glDisable(GL_DITHER); // Дизер цвета не юзать
  glDisable(GL_FOG); // Туман не нужен
  glDisable(GL_STENCIL_TEST); // трафарет чекать - лишнее
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST); // простое преобразование кординат

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

  glClear(GL_COLOR_BUFFER_BIT);

  // Вычисляем индекс следующего буфера для работы CPU
  int next_pbo_index = (m_pbo_index + 1) % 2;

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_screen_tex);

  // ШАГ 1: Асинхронно отправляем данные ТЕКУЩЕГО PBO в текстуру экрана
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbos[m_pbo_index]);
  
  // nullptr в конце означает: брать данные со смещением 0 внутри привязанного PBO.
  // Это триггерит сверхбыстрый DMA-трансфер в видеопамять.
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_w, m_h, GL_RED, GL_UNSIGNED_BYTE, nullptr);

  // ШАГ 2: Загружаем данные для СЛЕДУЮЩЕГО кадра в другой PBO с помощью CPU
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbos[next_pbo_index]);
  
  // Флаг GL_MAP_INVALIDATE_BUFFER_BIT сбрасывает старую память буфера.
  // Драйвер мгновенно выдает CPU новый кусок памяти, не дожидаясь рендеринга старого кадра на GPU.
  void* mapped_ptr = glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, m_pbo_sz, 
                                      GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
  if (mapped_ptr) {
    std::memcpy(mapped_ptr, m_pixels, m_pbo_sz);
    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
  }

  // Обязательно отвязываем PBO перед вызовом отрисовки
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

  // Стандартная отрисовка палитры и полигона
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_1D, m_pal_tex);

  glBindVertexArray(m_vao);
  glUseProgram(m_shader_prog);

  glDrawArrays(GL_TRIANGLES, 0, 3); 

  glUseProgram(0);
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_1D, 0);
  glBindTexture(GL_TEXTURE_2D, 0);

  if (!graphic::double_buffering) {
    glFlush();
  }

  // Ротация буферов. const_cast нужен, так как метод draw_game_frame() объявлен как const.
  const_cast<Host_ogl*>(this)->m_pbo_index = next_pbo_index;
} // draw_game_frame

// вызывается в конце инита наследника
void Host_ogl::ogl_post_init() {
  ogl_init();
  reshape(m_window_ctx.w, m_window_ctx.h);
}

void Host_ogl::init_screen_poly() {
  // VBO не юзать, сделать пустой VAO
  glGenVertexArrays(1, &m_vao);
  glBindVertexArray(m_vao);

  glBindVertexArray(0); 
}

void Host_ogl::compile_program() {
  m_shader_prog = glCreateProgram();
  glAttachShader(m_shader_prog, m_vert_shader);
  glAttachShader(m_shader_prog, m_frag_shader);

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
    log_debug << "Shader program info: " << Str(info_msg.begin(), info_msg.end());
  }
} // compile_program

void Host_ogl::compie_vertex_shader() {
  auto source_file = load_res("graphic/shaders/default.vert");
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
    log_debug << "Vertex shader info: " << Str(info_msg.begin(), info_msg.end());
  }
} // compie_vertex_shader

void Host_ogl::compie_fragment_shader() {
  auto source_file = load_res("graphic/shaders/default.frag");
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
    log_debug << "Fragment shader info: " << Str(info_msg.begin(), info_msg.end());
  }
} // compie_fragment_shader

void Host_ogl::screen_tex_init() {
  check_max_gltex_sz();
  glGenTextures(1, &m_screen_tex);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_screen_tex);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
  // КРИТИЧЕСКОЕ ИСПРАВЛЕНИЕ: Выделяем честный 8-битный 1-канальный формат под индекс палитры
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, m_w, m_h, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
  glBindTexture(GL_TEXTURE_2D, 0);

  // ИНИЦИАЛИЗАЦИЯ ДВОЙНОГО PBO
  m_pbo_sz = m_w * m_h; // 1 байт на пиксель для индексированного режима
  glGenBuffers(2, m_pbos);
  
  for (int i = 0; i < 2; ++i) {
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbos[i]);
    // Режим GL_STREAM_DRAW сообщает драйверу схему "Запись CPU -> Чтение GPU"
    glBufferData(GL_PIXEL_UNPACK_BUFFER, m_pbo_sz, nullptr, GL_STREAM_DRAW);
  }
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
} // screen_tex_init

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
      log_warning << "не удалось загрузить файл палитры \"" <<
        graphic::current_palette_file << "\". Будет выбрана стандартная палитра";
      graphic::current_palette_file = graphic::DEFAULT_PALETTE_FILE;
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
  log_debug << "max texture size: " << avaliable_max_sz;
  // если эта ошибка выстрелит, то напиши разбивку на тайлы и их рендери
  iferror(avaliable_max_sz < max_canvas_sz,
    "GL_MAX_TEXTURE_SIZE < " << n2s(max_canvas_sz));
}
