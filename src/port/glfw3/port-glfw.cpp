#include "pch.hpp"
#include "port-glfw.hpp"

#include <csignal>
#include "stb/stb_image.h"

#ifdef WINDOWS
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>

  #define GLFW_DLL
#else
  #include <unistd.h>
#endif

#include <glad/gl.h>
// GLAD ниже GLFW не ставить. GL/glcorearb.h не подключать
#include <GLFW/glfw3.h>

#include "engine/graphic/image/color-convert.hpp"
#include "engine/graphic/image/color-rgb.hpp"
#include "engine/graphic/image/palette.hpp"
#include "game/util/resource-helper.hpp"
#include "game/util/screenshot.hpp"
#include "game/util/version.hpp"
#include "game/util/logo.hpp"
#include "game/core/core-window.hpp"
#include "game/core/graphic.hpp"
#include "game/core/palette.hpp"
#include "game/core/common.hpp"
#include "game/core/canvas.hpp"
#include "game/input/keys.hpp"
#include "port/port.hpp"

#ifndef DISABLE_CONFIG
  #include "game/util/game-archive.hpp"
  #include "game/util/config.hpp"
#endif


//---- Все нужные данные для GLFW окна -----------------------------------------
struct Glfw3_context {
  int argc {};
  char** argv {};

// GLFW окно:
  GLFWwindow* window {};
  Veci wnd_off {440, 240}; // Стартовое смещение окна
  Veci wnd_sz {graphic::width, graphic::height}; // реальная ширина и высота окна (меняется при ресайзе)
  Recti wnd_game_area {}; // видимая часть внутреннего окна игры
  Str window_title {"H.P.W"};
  bool start_fullscreen {false}; // через аргументы запуска можно указать запуститься в фулскрине
#ifdef WINDOWS
  void* app_mutex {};
#endif

// OpenGL:
  Vector<GLfloat> pal_rgb {}; // Палитра игры подготовленная к отправке на видяху
  uint screen_tex = 0; // OpenGL текстура экрана
  uint vao = 0;
  GLuint pbos[2] = {0, 0};
  bool use_double_pbo {true};
  int pbo_index = 0;
  size_t pbo_sz = 0;
  uint shader_prog = 0, vert_shader = 0, frag_shader = 0;
  uint pal_tex = 0;
  cp<void> pixels {}; // данные для копирования в текстуру

// Общее для игры:
  bool is_ran {true};
  bool rebind_key_mode {false};
  hpw::keycode key_for_rebind {hpw::keycode::error}; // появится при hpw::rebind_key
  bool once_gamma_conf {true}; // защита от циклического выставления гаммы
  std::optional<uint32_t> custom_seed {};
};
static Glfw3_context g_ctx {};

//---- Внутренние функции порта ------------------------------------------------

static void init_rgb_pal() {
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

static Rgb24 pal8_to_rgb24_by_file(const Pal8 x) {
  if ( !g_ctx.pal_rgb.empty())
    return to_rgb24(
      g_ctx.pal_rgb.at(x.val * 3 + 0) * 255,
      g_ctx.pal_rgb.at(x.val * 3 + 1) * 255,
      g_ctx.pal_rgb.at(x.val * 3 + 2) * 255
    );
  return to_rgb24(x);
}

static Vector<GLfloat> load_ogl_palette(cr<Str> fname) {
  // загрузка png
  int x, y;
  int comp; // сколько цветовых каналов
  cauto mem = load_res(fname);
  cauto mem_data = cptr2ptr<cp<stbi_uc>>(mem.data.data());
  auto decoded = stbi_load_from_memory(mem_data, mem.data.size(), &x, &y, &comp, STBI_rgb);
  iferror( !decoded, "image data is not decoded");
  iferror(x < 256, "ширина картинки должна быть не меньше 256");

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

static void set_palette(cr<Str> fname) {
  graphic::current_palette_file = fname.empty()
    ? Str{graphic::DEFAULT_PALETTE_FILE}
    : fname;
  
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

static void check_max_gltex_sz() {
  auto max_canvas_sz = std::max(graphic::width, graphic::height);
  GLint avaliable_max_sz = -1;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &avaliable_max_sz);
  log_debug << "max texture size: " << avaliable_max_sz;
  // если эта ошибка выстрелит, то напиши разбивку на тайлы и их рендери
  iferror(avaliable_max_sz < max_canvas_sz,
    "GL_MAX_TEXTURE_SIZE < " << n2s(max_canvas_sz));
}

static void screen_tex_init() {
  check_max_gltex_sz();
  glGenTextures(1, &g_ctx.screen_tex);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, g_ctx.screen_tex);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, g_ctx.wnd_sz.x, g_ctx.wnd_sz.y, 0, GL_RED,
    GL_UNSIGNED_BYTE, nullptr);
  glBindTexture(GL_TEXTURE_2D, 0);

  // Двойной PBO
  static_assert(sizeof(Pal8) == 1, "g_ctx.pbo_sz ожидает что пиксели весят 1 байт");
  g_ctx.pbo_sz = g_ctx.wnd_sz.x * g_ctx.wnd_sz.y;
  glGenBuffers(2, g_ctx.pbos);
  
  cfor (i, 2) {
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, g_ctx.pbos[i]);
    // Режим GL_STREAM_DRAW сообщает драйверу схему "Запись CPU -> Чтение GPU"
    glBufferData(GL_PIXEL_UNPACK_BUFFER, g_ctx.pbo_sz, nullptr, GL_STREAM_DRAW);
  }

  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

static void compile_program() {
  g_ctx.shader_prog = glCreateProgram();
  glAttachShader(g_ctx.shader_prog, g_ctx.vert_shader);
  glAttachShader(g_ctx.shader_prog, g_ctx.frag_shader);

  glLinkProgram(g_ctx.shader_prog);
  glUseProgram(g_ctx.shader_prog);
  glUniform1i(glGetUniformLocation(g_ctx.shader_prog, "main_tex"), 0);
  glUniform1i(glGetUniformLocation(g_ctx.shader_prog, "pal_tex"), 1);

  // check
  GLint result;
  int info_len;
  glGetProgramiv(g_ctx.shader_prog, GL_LINK_STATUS, &result);
  glGetProgramiv(g_ctx.shader_prog, GL_INFO_LOG_LENGTH, &info_len);
  if (info_len > 0) {
    Vector<char> info_msg(info_len + 1);
    glGetProgramInfoLog(g_ctx.shader_prog, info_len, {}, info_msg.data());
    log_debug << "Shader program info: " << Str(info_msg.begin(), info_msg.end());
  }
}

static void compie_vertex_shader() {
  auto source_file = load_res("graphic/shaders/default.vert");
  auto source_text = Str(source_file.data.begin(), source_file.data.end());
  auto source = scast<cp<GLchar>>(source_text.c_str());
  g_ctx.vert_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(g_ctx.vert_shader, 1, &source, 0);
  glCompileShader(g_ctx.vert_shader);
  // check
  GLint result;
  int info_len;
  glGetShaderiv(g_ctx.vert_shader, GL_COMPILE_STATUS, &result);
  glGetShaderiv(g_ctx.vert_shader, GL_INFO_LOG_LENGTH, &info_len);
  if (info_len > 0) {
    Vector<char> info_msg(info_len + 1);
    glGetShaderInfoLog(g_ctx.vert_shader, info_len, {}, info_msg.data());
    log_debug << "Vertex shader info: " << Str(info_msg.begin(), info_msg.end());
  }
}

static void compie_fragment_shader() {
  auto source_file = load_res("graphic/shaders/default.frag");
  auto source_text = Str(source_file.data.begin(), source_file.data.end());
  auto source = scast<cp<GLchar>>(source_text.c_str());
  g_ctx.frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(g_ctx.frag_shader, 1, &source, 0);
  glCompileShader(g_ctx.frag_shader);
  // check
  GLint result;
  int info_len;
  glGetShaderiv(g_ctx.frag_shader, GL_COMPILE_STATUS, &result);
  glGetShaderiv(g_ctx.frag_shader, GL_INFO_LOG_LENGTH, &info_len);
  if (info_len > 0) {
    Vector<char> info_msg(info_len + 1);
    glGetShaderInfoLog(g_ctx.frag_shader, info_len, {}, info_msg.data());
    log_debug << "Fragment shader info: " << Str(info_msg.begin(), info_msg.end());
  }
}

static void init_screen_poly() {
  // VBO не юзать, сделать пустой VAO
  glGenVertexArrays(1, &g_ctx.vao);
  glBindVertexArray(g_ctx.vao);

  glBindVertexArray(0); 
}

static bool is_bad_wnd_sz() {
  return
    g_ctx.wnd_sz.x <= 0 ||
    g_ctx.wnd_sz.y <= 0 ||
    g_ctx.wnd_game_area.size.x <= 0 ||
    g_ctx.wnd_game_area.size.y <= 0; 
}

// растягивает внутреннее окно игры с учётом соотношений и режимов
static void apply_resize_mode() {
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

static void soft_exit() {
  log_info << "software exit...";
  g_ctx.is_ran = false;
}

static void reshape(int w, int h) {
  g_ctx.wnd_sz.x = w;
  g_ctx.wnd_sz.y = h;
  apply_resize_mode();

  // окно нулевого размера не надо отображать
  graphic::enable_render = !is_bad_wnd_sz();
  if (graphic::enable_render) {
    // настройка 2D координат и проекций OpenGL:
    glViewport(
      g_ctx.wnd_game_area.pos.x,
      g_ctx.wnd_game_area.pos.y,
      g_ctx.wnd_game_area.pos.x + g_ctx.wnd_game_area.size.x,
      g_ctx.wnd_game_area.pos.y + g_ctx.wnd_game_area.size.y);
  }

  log_debug << "new window size: " << g_ctx.wnd_sz.x << ", " << g_ctx.wnd_sz.y
    << " (area: " << to_str(g_ctx.wnd_game_area.pos) << ", " << to_str(g_ctx.wnd_game_area.size) << ")";
}

static void init_opengl() {
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

  compie_vertex_shader();
  compie_fragment_shader();
  compile_program();

  init_screen_poly();
  reshape(g_ctx.wnd_sz.x, g_ctx.wnd_sz.y);
}

static void set_resize_mode(Resize_mode mode) {
  log_debug << "new resize mode: " << scast<int>(mode);
  graphic::resize_mode = mode;
  reshape(graphic::width, graphic::height);
  
  /* если фулскрин был включён, то сначала выключить его,
  иначе на линуксе экран погаснет */
  if (graphic::fullscreen) {
    Port::toggle_fullscreen(false);
    Port::toggle_fullscreen(true);
  }
}

// привязка функций порта
static void init_port_cbs() {
  log_debug << "init port callbacks...";

  Port::soft_exit = &soft_exit;
  Port::make_screenshot = []{ save_screenshot(graphic::canvas); };
  Port::set_resize_mode = &set_resize_mode;
  Port::set_palette = &set_palette;
  /* TODO
  Port::rebind_key = ;
  Port::rebind_key_by_scancode = ;
  Port::reset_keymap = ;
  Port::toggle_vsync = ;
  Port::set_gamma = ;
  Port::toggle_fullscreen = ;
  Port::toggle_double_buffering = ;
  Port::toggle_mouse_visible = ;
  Port::set_target_ups = ;
  Port::get_time = ;
  */
}

// рандомный логотип в консоли
static void print_logo() {
  cauto print_source = hpw::logger.config.print_source;
  cauto print_stream_name = hpw::logger.config.print_stream_name;
  hpw::logger.config.print_source = false;
  hpw::logger.config.print_stream_name = false;

  log_info << get_random_logo();

  hpw::logger.config.print_source = print_source;
  hpw::logger.config.print_stream_name = print_stream_name;
}

// настроить сид рандома
static void init_random() {
  log_debug << "init random seed...";

  u32_t seed;
  if (g_ctx.custom_seed.has_value()) {
    seed = g_ctx.custom_seed.value();
  } else {
    seed = std::time(nullptr);
  }

  // TODO учесть большой сид
  log_debug << std::format("seed: {}\n", seed);
  set_rnd_seed({.old_seed=seed});
}

// для предупреждения о двух окнах игры
static void init_app_mutex() {
#ifdef WINDOWS
  g_ctx.app_mutex = ::CreateMutex(NULL, TRUE, "HPW: Double window mutex");
  if (::GetLastError() == ERROR_ALREADY_EXISTS) {
    hpw::multiple_apps = true;
    ::CloseHandle(g_ctx.app_mutex);
    g_ctx.app_mutex = nullptr;
    log_warning << "Запущено больше двух экземпляров игры";
  }
#else
  #pragma message("need impl for other_game_started() on Linux")
#endif
}

static void free_app_mutex() {
#ifdef WINDOWS
  if (g_ctx.app_mutex) {
    ::ReleaseMutex(g_ctx.app_mutex);
    ::CloseHandle(g_ctx.app_mutex);
  }
#else
  #pragma message("need impl for free_app_mutex() on Linux")
#endif
}

// Обработчик для Ctrl+C
static void signal_handler(int signal) {
  if (signal == SIGINT) {
    log_info << "Закрытие через SIGINT...";
    g_ctx.is_ran = false;
  }
}

static void process_args(int argc, char** argv) {
  hpw::argc = g_ctx.argc = argc;
  hpw::argv = g_ctx.argv = argv;

  Pparser ret( Pparser::v_param_t {
    {{"-s", "--seed"}, "set random seed", [](cr<Str> val){
      g_ctx.custom_seed = s2n<u32_t>(val);
    }},
    {{"-ox", "--offset_x"}, "set window startup offset by x", [](cr<Str> val){
      g_ctx.wnd_off.x = s2n<int>(val);
    }},
    {{"-oy", "--offset_y"}, "set window startup offset by y", [](cr<Str> val){
      g_ctx.wnd_off.y = s2n<int>(val);
    }},
    {{"-w", "--windowed"}, "enable windowed mode", [](cr<Str> val){
      g_ctx.start_fullscreen = false;
    }},
    {{"-f", "--fullscreen"}, "enable fullscreen mode", [](cr<Str> val){
      g_ctx.start_fullscreen = true;
    }},
    {{"-d", "--debug"}, "enable debug log stream", [](cr<Str> _){
      hpw::logger.config.use_stream_debug = true;
      hpw::forced_debug_log = true;
    }},
    {{"-h", "--help", "--info"}, "print this help and exit", [&](cr<Str> val){
      ret.print_info();
      std::exit(EXIT_SUCCESS);
    }},
    {{"-t", "--title"}, "set window title (if not defined, set HPW)", [](cr<Str> val) {
      if (!val.empty())
        g_ctx.window_title = val;
    }},
    {{"-v", "--version"}, "print game version and exit", [](cr<Str> val){
      cauto ver = get_game_version();
      cauto creation_date = get_game_creation_date();
      cauto creation_time = get_game_creation_time();
      log_info << std::format("game version: {} ({} {})", ver, creation_date, creation_time);
      std::exit(EXIT_SUCCESS);
    }},
  } );

  ret.skip_empty = true;
  ret(argc, argv);
}

// перегоняет пиксели игры в OpenGL текстуру
static void draw_game_frame() {
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
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, g_ctx.wnd_sz.x, g_ctx.wnd_sz.y, GL_RED,
      GL_UNSIGNED_BYTE, nullptr);

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
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, g_ctx.wnd_sz.x, g_ctx.wnd_sz.y, GL_RED,
      GL_UNSIGNED_BYTE, nullptr);

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

//---- Основные функции GLFW3 порта --------------------------------------------
void glfw3_init(int argc, char** argv) {
  log_info << "GLFW3 initialization...";

  std::signal(SIGINT, signal_handler); // для закрытия через Ctrl+C 

#ifdef WINDOWS
  // переключить консоль винды в UTF8
  SetConsoleOutputCP(CP_UTF8);
#endif

  process_args(argc, argv); // парс аргументов <--------------------------------
  hpw::cur_dir = launch_dir_from_argv0(argv); // узнать в какой папке игра запущена
  log_debug << "launch directory: " << hpw::cur_dir;

  init_app_mutex();
  init_random();
  print_logo();
#ifndef DISABLE_CONFIG
  load_config();
#endif
  init_port_cbs();

  g_ctx.pixels = scast<cp<void>>(graphic::canvas.data());

#ifndef DISABLE_CONFIG
  init_archive(); // из архива понадобятся шейдеры
#endif
#ifndef ECOMEM
  load_color_tables();
#endif

  log_debug << "GLFW3 end of initialization";
}

bool glfw3_is_ran() {
  return g_ctx.is_ran && g_ctx.window && (glfwWindowShouldClose(g_ctx.window) == GL_FALSE);
}

void glfw3_update() {
  // TODO update states
}

void glfw3_render_canvas() {
  // TODO
}

void glfw3_sync() {
  // TODO
}

void glfw3_finalize() {
  log_debug << "GLFW3 finalization...";
  
#ifndef DISABLE_CONFIG
  save_config();
#endif
  
  free_app_mutex();
  
  log_info << "Корректное завершение";
}
