#include "pch.hpp"
#include "glfw-render.hpp"
#include "glfw-context.hpp"

#include <type_traits>
#include "stb/stb_image.h"

#include "engine/graphic/image/color-convert.hpp"
#include "engine/graphic/image/color-rgb.hpp"
#include "engine/graphic/image/palette.hpp"
#include "game/util/resource-helper.hpp"
#include "game/util/game-title.hpp"
#include "game/util/logo.hpp"
#include "game/util/sync.hpp"
#include "game/core/graphic.hpp"
#include "game/core/palette.hpp"
#include "game/core/user.hpp"
#include "port/port.hpp"


static Str get_window_name() {
  if (!g_ctx.window_title.empty())
    return g_ctx.window_title;
  return hpw::default_tile.empty()
    ? rnd_game_title()
    : hpw::default_tile;
}

static void init_icon() {
  GLFWimage icon;
  icon.pixels = nullptr;

  try {
    // случайно выбрать ярлык окна
    sconst Strs icon_names {
      "graphic/images/icons/128x128/Alles.png",
      "graphic/images/icons/128x128/boo.png",
      "graphic/images/icons/128x128/pattern 1.png",
      "graphic/images/icons/128x128/pattern 2.png",
      "graphic/images/icons/128x128/pattern 3.png",
    };
    cauto icon_name = icon_names.at(rndu_fast(icon_names.size() - 1));
    cauto icon_file = load_res(icon_name);
    int channels;
    icon.pixels = stbi_load_from_memory (
      rcast<cp<stbi_uc>>(icon_file.data.data()), icon_file.data.size(),
      &icon.width, &icon.height, &channels, 4
    );
    iferror(channels != 4, "цветовых каналов в изображении " << n2s(channels) << ", а нужно 4");
    glfwSetWindowIcon(g_ctx.window, 1, &icon); 
  } catch (cr<hpw::Error> err) {
    log_error << "не удалось установить иконку для окна: " << err.what();
  } catch (...) {
    log_error << "не удалось установить иконку для окна. Неизвестная ошибка";
  }

  stbi_image_free(icon.pixels);
}

void draw_game_frame() {
  ret_if (!graphic::enable_render);

  glClear(GL_COLOR_BUFFER_BIT);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, g_ctx.screen_tex);

  if (g_ctx.use_double_pbo) {
    const int next_pbo_index = (g_ctx.pbo_index + 1) % 2;

    // Пока готовим текущий PBO, отправляем на видяху предыдущий
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, g_ctx.pbos[g_ctx.pbo_index]);
    /* nullptr в конце означает: брать данные со смещением 0 внутри
    привязанного PBO. Это триггерит сверхбыстрый DMA-трансфер в видеопамять. */
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, graphic::width, graphic::height,
      GL_RED, GL_UNSIGNED_BYTE, nullptr);

    // Грузим текущий кадр в PBO
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, g_ctx.pbos[next_pbo_index]);
    /* Флаг GL_MAP_INVALIDATE_BUFFER_BIT сбрасывает старую память буфера.
    Драйвер мгновенно выдает CPU новый кусок памяти,
    не дожидаясь рендеринга старого кадра на GPU. */
    void* mapped_ptr = glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, g_ctx.pbo_sz, 
      GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

    if (mapped_ptr) {
      std::memcpy(mapped_ptr, g_ctx.pixels, g_ctx.pbo_sz);
      glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
    }

    // отвязываем PBO перед вызовом отрисовки
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    // Ротация буферов
    g_ctx.pbo_index = next_pbo_index;
  } else { // Режим без двойного PBO
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, g_ctx.pbos[0]);
    
    // Загружаем текущие пиксели прямо сейчас
    void* mapped_ptr = glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, g_ctx.pbo_sz, 
      GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

    if (mapped_ptr) {
      std::memcpy(mapped_ptr, g_ctx.pixels, g_ctx.pbo_sz);
      glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
    }
    
    // Сразу же принудительно отправляем их в текстуру и ждем
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, graphic::width, graphic::height,
      GL_RED, GL_UNSIGNED_BYTE, nullptr);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
  }

  // Отрисовка полигона и палитры
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_1D, g_ctx.pal_tex);

  glBindVertexArray(g_ctx.vao);
  glUseProgram(g_ctx.shader_prog);

  // Треугольник на весь экран, в котором текстура игры
  glDrawArrays(GL_TRIANGLES, 0, 3); 

  glUseProgram(0);
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_1D, 0);
  glBindTexture(GL_TEXTURE_2D, 0);

  if (!graphic::double_buffering)
    glFlush();
}

void cleanup_opengl() {
  ret_if (!g_ctx.window);

  glfwMakeContextCurrent(g_ctx.window);

  if (g_ctx.shader_prog) {
    glDeleteProgram(g_ctx.shader_prog);
    g_ctx.shader_prog = 0;
  }
  if (g_ctx.vert_shader) {
    glDeleteShader(g_ctx.vert_shader);
    g_ctx.vert_shader = 0;
  }
  if (g_ctx.frag_shader) {
    glDeleteShader(g_ctx.frag_shader);
    g_ctx.frag_shader = 0;
  }
  if (g_ctx.vao) {
    glDeleteVertexArrays(1, &g_ctx.vao);
    g_ctx.vao = 0;
  }
  if (g_ctx.pbos[0] || g_ctx.pbos[1]) {
    glDeleteBuffers(2, g_ctx.pbos);
    g_ctx.pbos[0] = 0;
    g_ctx.pbos[1] = 0;
  }
  if (g_ctx.screen_tex) {
    glDeleteTextures(1, &g_ctx.screen_tex);
    g_ctx.screen_tex = 0;
  }
  if (g_ctx.pal_tex) {
    glDeleteTextures(1, &g_ctx.pal_tex);
    g_ctx.pal_tex = 0;
  }

  g_ctx.pbo_index = 0;
}

void draw_startup_screen() {
  // рисуем надпись о загрузке:
  constexpr uint SCALE = 2;
  constexpr uint OFF_X = 30;
  constexpr uint OFF_Y = 30;
  constexpr uint LT_W = 38;
  constexpr uint LT_H = 4;
  scauto LOADING_TITLE =
    "#     #    #   ##   #  ##    ##       " 
    "#    # #  # #  # #     # #  #         " 
    "#    # #  ###  # #  #  # #  # #       " 
    "###   #   # #  ##   #  # #   ##  # # #";
  graphic::canvas.fill({});
  cfor (y, LT_H)
  cfor (x, LT_W) {
    cauto c = Pal8::from_real(LOADING_TITLE[y * LT_W + x] == ' ' ? 0.0 : 0.75);
    cfor (sy, SCALE)
    cfor (sx, SCALE) {
      cauto _x = x * SCALE + sx;
      cauto _y = y * SCALE + sy;
      graphic::canvas.set(OFF_X + _x, OFF_Y + _y, c, {});
    }
  }

  // их три, чтобы увидеть что-то при двойной буфферизации
  draw_game_frame(), glfwSwapBuffers(g_ctx.window);
  draw_game_frame(), glfwSwapBuffers(g_ctx.window);
  draw_game_frame(), glfwSwapBuffers(g_ctx.window);
}

void init_window() {
  log_info << "создание окна игры...";
  if (g_ctx.window) { // на случай реинита
    cleanup_opengl();
    glfwDestroyWindow(g_ctx.window);
    g_ctx.window = nullptr;
  }

  // настройки для контекста OGL
  glfwWindowHint(GLFW_DEPTH_BITS, 0);
  glfwWindowHint(GLFW_STENCIL_BITS, 0);
  glfwWindowHint(GLFW_ALPHA_BITS, 0);
  glfwWindowHint(GLFW_ACCUM_RED_BITS, 0);
  glfwWindowHint(GLFW_ACCUM_GREEN_BITS, 0);
  glfwWindowHint(GLFW_ACCUM_BLUE_BITS, 0);
  glfwWindowHint(GLFW_ACCUM_ALPHA_BITS, 0);
  glfwWindowHint(GLFW_AUX_BUFFERS, 0);
  glfwWindowHint(GLFW_STEREO, 0);
  glfwWindowHint(GLFW_SAMPLES, 0);
  glfwWindowHint(GLFW_SRGB_CAPABLE, 0);
  // выбор версии OGL
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API); // Не GLES
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // всё что ниже OGL3.3 лагает на встройке
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // старые функции вырубить
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  if (graphic::start_focused) {
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
    glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
  }
  // вкл/выкл даблбуфер
  glfwWindowHint (
    GLFW_DOUBLEBUFFER,
    graphic::double_buffering
      ? GL_TRUE
      : GL_FALSE
  ); 
  g_ctx.window = glfwCreateWindow(g_ctx.wnd_sz.x, g_ctx.wnd_sz.y,
    get_window_name().c_str(), nullptr, nullptr);
  iferror(!g_ctx.window, "g_ctx.window is null");
  glfwSetWindowPos(g_ctx.window, g_ctx.wnd_off.x, g_ctx.wnd_off.y);
  glfwMakeContextCurrent(g_ctx.window);

  log_debug << "GLAD2 init...";
  int version = gladLoadGL(glfwGetProcAddress);
  iferror(version == 0, "GLAD2 init error");
  log_debug << "GLAD2 version: " << version;

  // подгрузка расширений OGL
  cauto ogl_ver_cstr = glGetString(GL_VERSION);
  iferror( !ogl_ver_cstr, "не удалось получить версию OpenGL\n");
  const Str ogl_ver( cptr2ptr<Cstr>(ogl_ver_cstr) );
  log_debug << "OpenGL version: " << ogl_ver;

  // заливка окна
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT);
  glFinish();
  glfwSwapBuffers(g_ctx.window);

  init_opengl();
  draw_startup_screen();

  glfwSetWindowSizeCallback(g_ctx.window, reshape_cb);
  safe_call(Port::set_resize_mode, graphic::resize_mode);
  safe_call(Port::toggle_fullscreen, graphic::fullscreen);

  // поставить лимит фпс в 2 раза больше чем на экране
  auto monitor = glfwGetPrimaryMonitor();
  assert(monitor);
  auto monitor_mode = glfwGetVideoMode(monitor);
  assert(monitor_mode);

  // подстрока по частоте монитора
  auto monitor_framerate = monitor_mode->refreshRate;
  if (monitor_framerate > 0)
    graphic::set_target_vsync_fps(monitor_framerate);
  if (graphic::get_target_fps() == 0)
    graphic::set_target_fps(std::max(monitor_framerate * 2, 90));
  // определить растягивание окна по умолчанию
  if (monitor_mode->width > 0 && monitor_mode->height > 0) {
    if (monitor_mode->width == monitor_mode->height)
      graphic::default_resize_mode = Resize_mode::one_to_one;
    elif (monitor_mode->width > monitor_mode->height)
      graphic::default_resize_mode = Resize_mode::by_height;
    else
      graphic::default_resize_mode = Resize_mode::by_width;
  }

  // ивенты клавы
  glfwSetKeyCallback(g_ctx.window, &key_cb);
  // для чтения юникод текста при вводе
  glfwSetCharCallback(g_ctx.window, &utf32_text_input_cb);
  // настройка мыши
  safe_call(Port::toggle_mouse_visible, graphic::show_mouse_cursour);
  glfwSetMouseButtonCallback(g_ctx.window, &mouse_button_cb);

  init_icon();
}

void reshape_cb(GLFWwindow* _, int w, int h) { reshape(w, h); }

void print_logo() {
  cauto print_source = hpw::logger.config.print_source;
  cauto print_stream_name = hpw::logger.config.print_stream_name;
  hpw::logger.config.print_source = false;
  hpw::logger.config.print_stream_name = false;

  log_info << get_random_logo();

  hpw::logger.config.print_source = print_source;
  hpw::logger.config.print_stream_name = print_stream_name;
}

void toggle_mouse_visible(bool enable) {
  graphic::show_mouse_cursour = enable;
  if (g_ctx.window) {
    glfwSetInputMode(g_ctx.window, GLFW_CURSOR, enable
      ? GLFW_CURSOR_NORMAL
      : GLFW_CURSOR_HIDDEN);
  }
}

void toggle_fullscreen(bool enable) {
  log_debug << "new fullscreen mode: " << yn2s(enable);
  graphic::fullscreen = enable;
  ret_if (!g_ctx.window);

  if (enable) { // весь экран
    auto monitor = glfwGetPrimaryMonitor();
    auto mode = glfwGetVideoMode(monitor);
    glfwSetWindowMonitor(g_ctx.window, monitor, 0, 0,
      mode->width, mode->height, mode->refreshRate);
    reshape(mode->width, mode->height);
    graphic::toggle_vsync(graphic::get_vsync());
    set_gamma(graphic::gamma);
  } else { // окно
    g_ctx.wnd_sz = {graphic::width, graphic::height};
    glfwSetWindowMonitor(g_ctx.window, nullptr,
      g_ctx.wnd_off.x, g_ctx.wnd_off.y,
      g_ctx.wnd_sz.x, g_ctx.wnd_sz.y, GLFW_DONT_CARE);
    reshape(g_ctx.wnd_sz.x, g_ctx.wnd_sz.y);
    graphic::toggle_vsync(graphic::get_vsync());
  }
}

void toggle_double_buffering(bool enable) {
  log_debug << "new double-buffering mode: " << yn2s(enable);
  graphic::double_buffering = enable;
  if (g_ctx.window)
    init_window();
}

void update_viewport() {
  apply_resize_mode();

  graphic::enable_render = !is_bad_wnd_sz();
  if (graphic::enable_render)
    glViewport(
      g_ctx.wnd_game_area.pos.x,
      g_ctx.wnd_game_area.pos.y,
      g_ctx.wnd_game_area.size.x,
      g_ctx.wnd_game_area.size.y
    );

  log_debug << "window size: " << g_ctx.wnd_sz.x << ", " << g_ctx.wnd_sz.y
    << " (area: " << to_str(g_ctx.wnd_game_area.pos) << ", " << to_str(g_ctx.wnd_game_area.size) << ")";
}

void reshape(int w, int h) {
  ret_if (w <= 0 || h <= 0);

  g_ctx.wnd_sz.x = w;
  g_ctx.wnd_sz.y = h;
  update_viewport();
}

void init_opengl() {
  // отключаю ненужное для оптимизации
  glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);
  //glDisable(GL_LIGHT0);
  //glDisable(GL_LIGHTING);
  //glDisable(GL_COLOR_MATERIAL);
  glDisable(GL_BLEND);
  glDisable(GL_MULTISAMPLE); // отключение сглаживания
  //glShadeModel(GL_FLAT); // не гладкая заливка
  glDisable(GL_CULL_FACE); // без отсечения обратных граней полигона
  //glDisable(GL_ALPHA_TEST); // Альфа канал не чекать
  glDisable(GL_DITHER); // Дизер цвета не юзать
  //glDisable(GL_FOG); // Туман не нужен
  glDisable(GL_STENCIL_TEST); // трафарет чекать - лишнее
  //glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST); // простое преобразование кординат

  glClearColor(0,0,0,0);
  set_palette(graphic::current_palette_file);
  screen_tex_init();

  compile_vertex_shader();
  compile_fragment_shader();
  compile_program();

  init_screen_poly();
  reshape(g_ctx.wnd_sz.x, g_ctx.wnd_sz.y);
}

void set_resize_mode(Resize_mode mode) {
  log_debug << "new resize mode: " << scast<int>(mode);
  graphic::resize_mode = mode;
  update_viewport();
}

void set_gamma(double gamma) {
  ret_if(gamma == 1);

  graphic::gamma = std::clamp<double>(gamma, 0.025, 3);
  auto monitor = glfwGetPrimaryMonitor();
  glfwSetGamma(monitor, graphic::gamma);
}

void toggle_vsync(bool enable) {
  if (g_ctx.window) {
    log_debug << "V-Sync: " << yn2s(enable);
    glfwSwapInterval(enable ? 1 : 0);
  } else {
    log_debug << "Ignore V-Sync modification - window is not created";
  }
};

void init_rgb_pal() {
  log_debug << "init rgb (float) palette for OGL...";
  cauto default_pal = get_default_palette();
  g_ctx.pal_rgb.resize(default_pal.size() * 3);

  for (size_t i = 0; crauto c: default_pal) {
    g_ctx.pal_rgb.at(i * 3 + 0) = c.r / 255.0;
    g_ctx.pal_rgb.at(i * 3 + 1) = c.g / 255.0;
    g_ctx.pal_rgb.at(i * 3 + 2) = c.b / 255.0;
    ++i;
  }
}

Rgb24 pal8_to_rgb24_by_file(const Pal8 x) {
  if ( !g_ctx.pal_rgb.empty())
    return to_rgb24(
      g_ctx.pal_rgb.at(x.val * 3 + 0) * 255,
      g_ctx.pal_rgb.at(x.val * 3 + 1) * 255,
      g_ctx.pal_rgb.at(x.val * 3 + 2) * 255
    );
  return to_rgb24(x);
}

Vector<float> load_ogl_palette(cr<Str> fname) {
  static_assert(std::is_same_v<GLfloat, float>, "GLfloat != float");

  // загрузка png
  int x, y;
  int comp; // сколько цветовых каналов
  cauto mem = load_res(fname);
  cauto mem_data = cptr2ptr<cp<stbi_uc>>(mem.data.data());
  auto decoded = stbi_load_from_memory(mem_data, mem.data.size(),
    &x, &y, &comp, STBI_rgb);
  iferror( !decoded, "image data is not decoded");
  iferror(x < 256, "ширина картинки должна быть не меньше 256 пикселей");
  iferror(y < 1, "высота картинки должна быть не меньше 1 пикселя");

  // rgb -> palette array
  Vector<GLfloat> ogl_pal;
  int rgb_index = 0;
  for (int i = 0; i < x * y; ++i) {
    constexpr auto mul = (1.0 / 255.0);
    ogl_pal.push_back(decoded[rgb_index + 0] * mul);
    ogl_pal.push_back(decoded[rgb_index + 1] * mul);
    ogl_pal.push_back(decoded[rgb_index + 2] * mul);
    rgb_index += 3;
  }

  stbi_image_free(decoded);
  to_palette_rgb24 = &pal8_to_rgb24_by_file;
  return ogl_pal;
}

void set_palette(cr<Str> fname) {
  graphic::current_palette_file = fname.empty()
    ? Str{graphic::DEFAULT_PALETTE_FILE}
    : fname;
  
  // если текстура палитры есть - снести её
  if (g_ctx.pal_tex) {
    glDeleteTextures(1, &g_ctx.pal_tex);
    g_ctx.pal_tex = 0;
  }
  
  glGenTextures(1, &g_ctx.pal_tex);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_1D, g_ctx.pal_tex);
  // не делать линейным, иначе появятся розовые оттенки
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  // фиксит индекс палитры внутри шейдера
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  try {
    // загрузка палитры
    g_ctx.pal_rgb = load_ogl_palette(fname);
  } catch (...) {
    // если палитру не удастся получить, то сгенерировать её
    if (!fname.empty()) {
      log_warning << "не удалось загрузить файл палитры \"" <<
        fname << "\". Будет выбрана стандартная палитра";
      graphic::current_palette_file = graphic::DEFAULT_PALETTE_FILE;
    }
    init_rgb_pal();
  }

  // в 1D текстуре хранятся триплеты палитры для шейдера
  glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 256, 0, GL_RGB, GL_FLOAT, g_ctx.pal_rgb.data());
  glBindTexture(GL_TEXTURE_1D, 0); // unbind g_ctx.pal_tex
}

void check_max_gltex_sz() {
  auto max_canvas_sz = std::max(graphic::width, graphic::height);
  GLint avaliable_max_sz = -1;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &avaliable_max_sz);
  log_debug << "max texture size: " << avaliable_max_sz;
  // если эта ошибка выстрелит, то напиши разбивку на тайлы и их рендери
  iferror(avaliable_max_sz < max_canvas_sz,
    "GL_MAX_TEXTURE_SIZE < " << n2s(max_canvas_sz));
}

void screen_tex_init() {
  check_max_gltex_sz();
  glGenTextures(1, &g_ctx.screen_tex);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, g_ctx.screen_tex);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, graphic::width, graphic::height,
    0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
  glBindTexture(GL_TEXTURE_2D, 0);

  // Двойной PBO
  static_assert(sizeof(Pal8) == 1, "g_ctx.pbo_sz ожидает что пиксели весят 1 байт");
  g_ctx.pbo_sz = graphic::width * graphic::height; 
  glGenBuffers(2, g_ctx.pbos);
  
  cfor (i, 2) {
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, g_ctx.pbos[i]);
    // Режим GL_STREAM_DRAW сообщает драйверу схему "Запись CPU -> Чтение GPU"
    glBufferData(GL_PIXEL_UNPACK_BUFFER, g_ctx.pbo_sz, nullptr, GL_STREAM_DRAW);
  }

  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

void compile_program() {
  g_ctx.shader_prog = glCreateProgram();
  glAttachShader(g_ctx.shader_prog, g_ctx.vert_shader);
  glAttachShader(g_ctx.shader_prog, g_ctx.frag_shader);

  glLinkProgram(g_ctx.shader_prog);
  glUseProgram(g_ctx.shader_prog);
  glUniform1i(glGetUniformLocation(g_ctx.shader_prog, "main_tex"), 0);
  glUniform1i(glGetUniformLocation(g_ctx.shader_prog, "pal_tex"), 1);

  GLint result;
  glGetProgramiv(g_ctx.shader_prog, GL_LINK_STATUS, &result);
  // данные шейдера можно теперь не держать в памяти
  glDetachShader(g_ctx.shader_prog, g_ctx.vert_shader);
  glDetachShader(g_ctx.shader_prog, g_ctx.frag_shader);
  glDeleteShader(g_ctx.vert_shader);
  glDeleteShader(g_ctx.frag_shader);
  g_ctx.vert_shader = 0;
  g_ctx.frag_shader = 0;  
  
  int info_len;
  glGetProgramiv(g_ctx.shader_prog, GL_INFO_LOG_LENGTH, &info_len);
  if (info_len > 0) {
    Vector<char> info_msg(info_len + 1);
    glGetProgramInfoLog(g_ctx.shader_prog, info_len, {}, info_msg.data());
    log_debug << "Shader program info: " << Str(info_msg.begin(), info_msg.end());
  }
}

void compile_vertex_shader() {
  auto source_file = load_res("graphic/shaders/default.vert");
  iferror(!source_file, "empty vertex shader");
  auto source_text = Str(source_file.data.begin(), source_file.data.end());
  auto source = scast<cp<GLchar>>(source_text.c_str());
  g_ctx.vert_shader = glCreateShader(GL_VERTEX_SHADER);
  iferror (g_ctx.vert_shader == 0, "Failed to create vertex shader object");
  glShaderSource(g_ctx.vert_shader, 1, &source, 0);
  glCompileShader(g_ctx.vert_shader);
  
  GLint result = GL_FALSE;
  glGetShaderiv(g_ctx.vert_shader, GL_COMPILE_STATUS, &result);

  int info_len = 0;
  glGetShaderiv(g_ctx.vert_shader, GL_INFO_LOG_LENGTH, &info_len);
  if (info_len > 0) {
    Vector<char> info_msg(info_len + 1);
    glGetShaderInfoLog(g_ctx.vert_shader, info_len, {}, info_msg.data());
    const Str info_str(info_msg.begin(), info_msg.end());
    iferror (result == GL_FALSE, "Vertex shader compilation failed:\n" + info_str);
    if (!info_str.empty())
      log_debug << "Vertex shader info: " << info_str;
  }
}

void compile_fragment_shader() {
  auto source_file = load_res("graphic/shaders/default.frag");
  iferror(!source_file, "empty fragment shader");
  auto source_text = Str(source_file.data.begin(), source_file.data.end());
  auto source = scast<cp<GLchar>>(source_text.c_str());
  g_ctx.frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
  iferror (g_ctx.frag_shader == 0, "Failed to create fragment shader object");
  glShaderSource(g_ctx.frag_shader, 1, &source, 0);
  glCompileShader(g_ctx.frag_shader);
  
  GLint result = GL_FALSE;
  glGetShaderiv(g_ctx.frag_shader, GL_COMPILE_STATUS, &result);

  int info_len = 0;
  glGetShaderiv(g_ctx.frag_shader, GL_INFO_LOG_LENGTH, &info_len);
  if (info_len > 0) {
    Vector<char> info_msg(info_len + 1);
    glGetShaderInfoLog(g_ctx.frag_shader, info_len, {}, info_msg.data());
    const Str info_str(info_msg.begin(), info_msg.end());
    iferror (result == GL_FALSE, "Fragment shader compilation failed:\n" + info_str);
    if (!info_str.empty())
      log_debug << "Fragment shader info: " << info_str;
  }
}

void init_screen_poly() {
  // VBO не юзать, сделать пустой VAO
  glGenVertexArrays(1, &g_ctx.vao);
  glBindVertexArray(g_ctx.vao);

  glBindVertexArray(0); 
}

bool is_bad_wnd_sz() {
  return
    g_ctx.wnd_sz.x <= 0 ||
    g_ctx.wnd_sz.y <= 0 ||
    g_ctx.wnd_game_area.size.x <= 0 ||
    g_ctx.wnd_game_area.size.y <= 0; 
}

void apply_resize_mode() {
  constexpr int base_w = graphic::width;
  constexpr int base_h = graphic::height;
  constexpr real aspect = scast<real>(base_w) / scast<real>(base_h);

  // если окно винды свернуть, то оно станет нулевого размера. Защита от вылета:
  if (g_ctx.wnd_sz.x <= 0 || g_ctx.wnd_sz.y <= 0) {
    g_ctx.wnd_game_area.pos = {0, 0};
    g_ctx.wnd_game_area.size = {0, 0};
    return;
  }

  Veci new_sz {};

  switch (graphic::resize_mode) {
    default:
    case Resize_mode::one_to_one: {
      // максимальный множитель, при котором игра целиком влезает в окно
      const int scale_x = g_ctx.wnd_sz.x / base_w;
      const int scale_y = g_ctx.wnd_sz.y / base_h;
      int scale = std::min(scale_x, scale_y);

      // выбираем наибольший кратный 2 множитель или 1
      if (scale >= 2)
        scale = (scale / 2) * 2; // нечётные -> чётные
      else
        scale = 1;

      new_sz.x = base_w * scale;
      new_sz.y = base_h * scale;
      break;
    }

    case Resize_mode::by_width: {
      new_sz.x = g_ctx.wnd_sz.x;
      new_sz.y = scast<int>(new_sz.x / aspect);
      break;
    }

    case Resize_mode::by_height: {
      new_sz.y = g_ctx.wnd_sz.y;
      new_sz.x = scast<int>(new_sz.y * aspect);
      break;
    }

    case Resize_mode::full: {
      new_sz = g_ctx.wnd_sz;
      break;
    }
  } // switch resize mode

  // Центрирование внутреннего окна игры в физическом внешнем:
  g_ctx.wnd_game_area.size = new_sz;
  g_ctx.wnd_game_area.pos.x = (g_ctx.wnd_sz.x - new_sz.x) / 2;
  g_ctx.wnd_game_area.pos.y = (g_ctx.wnd_sz.y - new_sz.y) / 2;
}
