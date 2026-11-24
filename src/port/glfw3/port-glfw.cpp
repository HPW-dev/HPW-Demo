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

#include "port/windows/windows-keycodes.hpp" // TODO add Linux keycode-file
#include "engine/graphic/image/color-convert.hpp"
#include "engine/graphic/image/color-rgb.hpp"
#include "engine/graphic/image/palette.hpp"
#include "game/util/resource-helper.hpp"
#include "game/util/screenshot.hpp"
#include "game/util/game-title.hpp"
#include "game/util/version.hpp"
#include "game/util/logo.hpp"
#include "game/util/sync.hpp"
#include "game/core/core-window.hpp"
#include "game/core/graphic.hpp"
#include "game/core/palette.hpp"
#include "game/core/common.hpp"
#include "game/core/canvas.hpp"
#include "game/core/tasks.hpp"
#include "game/core/user.hpp"
#include "game/core/core.hpp"
#include "game/input/keys.hpp"
#include "port/port.hpp"

#ifndef DISABLE_CONFIG
  #include "game/util/game-archive.hpp"
  #include "game/util/config.hpp"
#endif


//---- классы/контексты/глобалы ------------------------------------------------
// перенаправляет управление с GLFW в игру
class Key_mapper final {
  nocopy(Key_mapper);

  struct GLFW_key {
    utf32 name {};
    int scancode {}; // системный код клавиши
  };

  // <hpw key, GLFW key>
  std::unordered_map<hpw::keycode, GLFW_key> table {};

public:
  inline Key_mapper() { reset(); }
  ~Key_mapper() = default;
  void reset(); // поставить клавиши по умолчанию
  void bind(hpw::keycode dst_hpw_key, int glfw_key); // забиндить клавишу
  inline decltype(table)& get_table() { return table; }
  Keys_info get_info() const;
};

// отложенное нажатие кнопки
class Lazy_press final: public Task {
  hpw::keycode _keycode {};
  bool _kill_me_on_next_update {};

public:
  inline explicit Lazy_press(const hpw::keycode keycode): _keycode {keycode} {}

  inline void update(const Delta_time dt) override final { 
    if (!_kill_me_on_next_update) {
      press(_keycode);
      _kill_me_on_next_update = true;
    } else
      this->kill();
  }

  inline void on_end() {
    release(_keycode);
  }
};

// Все нужные данные для GLFW окна
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
  Key_mapper key_mapper {};

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

void Key_mapper::bind(hpw::keycode dst_hpw_key, int scancode) {
  rauto dst = table[dst_hpw_key];
  dst.scancode = scancode;
  utf32 name = n2s<utf32>(scancode);
  // получить читаемое название кнопки
  if (auto win_name = get_windows_keycode_name(scancode); win_name) {
    name = *win_name;
  } else { // если имя не найдено, то попытаться найти его в GLFW
    auto glfw_key_name = glfwGetKeyName(GLFW_KEY_UNKNOWN, dst.scancode);
    if (glfw_key_name)
      name = sconv<utf32>(Str(glfw_key_name)) + U" (" + n2s<utf32>(dst.scancode) + U")";
  }
  dst.name = name;
}

Keys_info Key_mapper::get_info() const {
  Keys_info info;
  for (crauto [hpw_key, glfw_key]: table) {
    info.keys.emplace_back(Keys_info::Item {
      .hpw_key = hpw_key,
      .name = glfw_key.name,
      .scancode = glfw_key.scancode
    });
  }
  return info;
}

void Key_mapper::reset() { 
  // бинды по умолчанию
  bind(hpw::keycode::up,           glfwGetKeyScancode(GLFW_KEY_UP));
  bind(hpw::keycode::down,         glfwGetKeyScancode(GLFW_KEY_DOWN));
  bind(hpw::keycode::left,         glfwGetKeyScancode(GLFW_KEY_LEFT));
  bind(hpw::keycode::right,        glfwGetKeyScancode(GLFW_KEY_RIGHT));
  bind(hpw::keycode::focus,        glfwGetKeyScancode(GLFW_KEY_LEFT_ALT));
  bind(hpw::keycode::shoot,        glfwGetKeyScancode(GLFW_KEY_Z));
  bind(hpw::keycode::bomb,         glfwGetKeyScancode(GLFW_KEY_X));
  bind(hpw::keycode::mode,         glfwGetKeyScancode(GLFW_KEY_A));
  bind(hpw::keycode::enable,       glfwGetKeyScancode(GLFW_KEY_ENTER));
  bind(hpw::keycode::escape,       glfwGetKeyScancode(GLFW_KEY_ESCAPE));

  bind(hpw::keycode::reset,        glfwGetKeyScancode(GLFW_KEY_F5));
  bind(hpw::keycode::fast_forward, glfwGetKeyScancode(GLFW_KEY_F4));
  bind(hpw::keycode::debug,        glfwGetKeyScancode(GLFW_KEY_F3));
  bind(hpw::keycode::console,      glfwGetKeyScancode(GLFW_KEY_GRAVE_ACCENT));
  bind(hpw::keycode::text_delete,  glfwGetKeyScancode(GLFW_KEY_BACKSPACE));
  bind(hpw::keycode::fps,          glfwGetKeyScancode(GLFW_KEY_F1));
  bind(hpw::keycode::fulscrn,      glfwGetKeyScancode(GLFW_KEY_F11));
  bind(hpw::keycode::screenshot,   glfwGetKeyScancode(GLFW_KEY_F2));
}

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

static void compile_program() {
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

static void compile_vertex_shader() {
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

static void compile_fragment_shader() {
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

static void update_viewport() {
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

static void reshape(int w, int h) {
  ret_if (w <= 0 || h <= 0);

  g_ctx.wnd_sz.x = w;
  g_ctx.wnd_sz.y = h;
  update_viewport();
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

  compile_vertex_shader();
  compile_fragment_shader();
  compile_program();

  init_screen_poly();
  reshape(g_ctx.wnd_sz.x, g_ctx.wnd_sz.y);
}

static void set_resize_mode(Resize_mode mode) {
  log_debug << "new resize mode: " << scast<int>(mode);
  graphic::resize_mode = mode;
  update_viewport();
}

static void set_gamma(const double gamma) {
  ret_if(gamma == 1);

  graphic::gamma = std::clamp<double>(gamma, 0.025, 3);
  auto monitor = glfwGetPrimaryMonitor();
  glfwSetGamma(monitor, graphic::gamma);
}

// включить режим переназначения кнопки
static void rebind_key(hpw::keycode hpw_key) {
  g_ctx.rebind_key_mode = true;
  g_ctx.key_for_rebind = hpw_key;
};

static void rebind_key_by_scancode(hpw::keycode hpw_key, int scancode){
  g_ctx.key_mapper.bind(hpw_key, scancode);
  hpw::keys_info = g_ctx.key_mapper.get_info();
};
  
static void reset_keymap() {
  log_debug << "reseting keymap...";
  g_ctx.key_mapper.reset();
  hpw::keys_info = g_ctx.key_mapper.get_info();
};

static void toggle_vsync(const bool enable) {
  log_debug << "V-Sync: " << yn2s(enable);
  glfwSwapInterval(enable ? 1 : 0);
};

static void set_target_ups(int new_ups) {
  assert(new_ups > 0);
  hpw::target_ups = new_ups;
  hpw::target_tick_time = 1.0 / hpw::target_ups;
}

static void toggle_mouse_visible(bool enable) {
  graphic::show_mouse_cursour = enable;
  glfwSetInputMode(g_ctx.window, GLFW_CURSOR, enable
    ? GLFW_CURSOR_NORMAL
    : GLFW_CURSOR_HIDDEN);
}

static void toggle_fullscreen(bool enable) {
  log_debug << "new fullscreen mode: " << yn2s(enable);
  graphic::fullscreen = enable;

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

static void init_window();

static void toggle_double_buffering(bool enable) {
  log_debug << "new double-buffering mode: " << yn2s(enable);
  graphic::double_buffering = enable;
  init_window();
}

// привязка функций порта
static void init_port_cbs() {
  log_debug << "init port callbacks...";

  // misc
  Port::make_screenshot = []{ save_screenshot(graphic::canvas); };
  Port::set_target_ups = &set_target_ups;
  Port::soft_exit = &soft_exit;
  Port::get_time = &glfwGetTime;
  
  // graphic
  Port::toggle_double_buffering = &toggle_double_buffering;
  Port::toggle_mouse_visible = &toggle_mouse_visible;
  Port::toggle_fullscreen = &toggle_fullscreen;
  Port::set_resize_mode = &set_resize_mode;
  Port::toggle_vsync = &toggle_vsync;
  Port::set_palette = &set_palette;
  Port::set_gamma = &set_gamma;

  // input
  Port::rebind_key_by_scancode = &rebind_key_by_scancode;
  Port::rebind_key = &rebind_key;
  Port::reset_keymap = &reset_keymap;
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

static void hotkey_process(GLFWwindow* window,
int key, int scancode, int action, int mods) {
  // альтернативная кнопка скриншота
  if (action == GLFW_PRESS && key == GLFW_KEY_PRINT_SCREEN)
    hpw::task_mgr.add(new_shared<Lazy_press>(hpw::keycode::screenshot));
  
  // альтернативная кнопка фуллскрина
  if (action == GLFW_PRESS && key == GLFW_KEY_ENTER && mods == GLFW_MOD_ALT) {
    hpw::task_mgr.add(new_shared<Lazy_press>(hpw::keycode::fulscrn));
    release(hpw::keycode::enable);
  }
  
  // вставка текста из буффера Ctrl + V
  if (action == GLFW_PRESS && key == GLFW_KEY_V && mods == GLFW_MOD_CONTROL) {
    if (hpw::text_input_mode) {
      cauto str_for_buf = glfwGetClipboardString(window);
      if (str_for_buf) {
        const Str buffer(str_for_buf);
        hpw::text_input.insert(hpw::text_input_pos, utf8_to_32(buffer));
        hpw::text_input_pos = std::min<int>(hpw::text_input_pos + buffer.size(),
          hpw::text_input.size());
      }
    }
  }

  // снос всей введёной строки текста (Ctrl + C)
  if (action == GLFW_PRESS && key == GLFW_KEY_C && mods == GLFW_MOD_CONTROL) {
    if (hpw::text_input_mode) {
      hpw::text_input.clear();
      hpw::text_input_pos = 0;
    }
  }
}

static void mouse_button_cb(GLFWwindow* window,
int button, int action, int mods) {
  // вставка текста из буффера при правой кнопке мыши
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
    if (hpw::text_input_mode) {
      const Str buffer = glfwGetClipboardString(window);
      if (!buffer.empty()) {
        hpw::text_input.insert(hpw::text_input_pos, utf8_to_32(buffer));
        hpw::text_input_pos = std::min<int>(hpw::text_input_pos + buffer.size(), hpw::text_input.size());
      }
    }
  }
}

static void key_cb(GLFWwindow* window, int key, int scancode,
int action, int mods) {
  hpw::any_key_pressed = true;
  rauto keymap_table = g_ctx.key_mapper.get_table();

  // режим ребинда клавиши
  if (g_ctx.rebind_key_mode) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
      g_ctx.key_mapper.bind(g_ctx.key_for_rebind, scancode);
      hpw::keys_info = g_ctx.key_mapper.get_info();
      g_ctx.rebind_key_mode = false;
      return; // чтобы нажатие не применилось в игровой логике
    }
  }

  // проверить нажатия на игровые клавиши
  for (crauto [hpw_key, key]: keymap_table) {
    if (key.scancode == scancode) {
      if (action == GLFW_PRESS || action == GLFW_REPEAT)
        press(hpw_key);
      else // GLFW_RELEASE
        release(hpw_key);
    }
  }

  hotkey_process(window, key, scancode, action, mods);
}

// колбэк для вывода ошибок GLFW
static void error_cb(int error, Cstr description) {
  // поставить дефолтную гамму при ошибке
  if (g_ctx.window && g_ctx.once_gamma_conf) {
    set_gamma(1);
    g_ctx.once_gamma_conf = false;
  }

  Str addition_desc;
  if (error == 65540)
    addition_desc += ". Maybe need delete config/config.json";

  error("GLFW error: " << error << ": " << description << addition_desc);
}

static void reshape_cb(GLFWwindow* /*g_ctx.window*/, int w, int h) {
  reshape(w, h);
}

// utf32 text input callback
static void utf32_text_input_cb(GLFWwindow* /*g_ctx.window*/, std::uint32_t codepoint) {
  return_if (!hpw::text_input_mode);

  hpw::text_input_pos = std::clamp<int>(hpw::text_input_pos, 0, hpw::text_input.size());
  cauto ch = scast<decltype(hpw::text_input)::value_type>(codepoint);
  hpw::text_input.insert(hpw::text_input_pos, 1, ch);
  hpw::text_input_pos = std::min<int>(hpw::text_input_pos + 1, hpw::text_input.size());
}

static void init_port(int argc, char** argv) {
  log_info << "Common port initialization...";

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

  set_target_ups(hpw::target_ups);
  graphic::set_target_fps(graphic::get_target_fps());
  hpw::safe_dt = graphic::get_target_frame_time();
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

static Str get_window_name() {
  return hpw::default_tile.empty()
    ? rnd_game_title()
    : hpw::default_tile;
}

static void draw_startup_screen() {
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

static void cleanup_opengl() {
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

static void init_window() {
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
  // клавиши надо стереть, чтобы при пересоздании окна не было залипания
  clear_cur_keys();
  // для чтения юникод текста при вводе
  glfwSetCharCallback(g_ctx.window, &utf32_text_input_cb);
  // настройка мыши
  safe_call(Port::toggle_mouse_visible, graphic::show_mouse_cursour);
  glfwSetMouseButtonCallback(g_ctx.window, &mouse_button_cb);

  init_icon();
}

//---- Основные функции GLFW3 порта --------------------------------------------
void glfw3_init(int argc, char** argv) {
  iferror(g_ctx.window, "не использовать два окна");
  init_port(argc, argv);

  log_debug << "GLFW3 initialization...";
  glfwSetErrorCallback(error_cb);
  iferror(!glfwInit(), "glfwInit error");
  init_window();

  reset_keymap();
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
  cleanup_opengl();

  if (g_ctx.window) {
    glfwDestroyWindow(g_ctx.window);
    g_ctx.window = nullptr;
  }

  glfwTerminate();
  
#ifndef DISABLE_CONFIG
  save_config();
#endif
  
  free_app_mutex();
  
  log_info << "Корректное завершение";
}
