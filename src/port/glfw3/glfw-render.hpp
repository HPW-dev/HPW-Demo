#pragma once
#include "game/core/core-window.hpp"
#include "util/vector-types.hpp"
#include "util/str.hpp"


struct GLFWwindow;
struct Rgb24;
struct Pal8;

// перегоняет пиксели игры в OpenGL текстуру
void draw_game_frame();
// очистка текстур и шейдеров OpenGL
void cleanup_opengl();
// пока порт грузится, надо хоть что-то показать
void draw_startup_screen();
// создать настроить окно игры
void init_window();
// рандомный логотип в консоли
void print_logo();
// растягивает внутреннее окно игры с учётом соотношений и режимов
void apply_resize_mode();

void reshape_cb(GLFWwindow* _, int w, int h);
void toggle_double_buffering(bool enable);
void toggle_fullscreen(bool enable);
void toggle_mouse_visible(bool enable);
void toggle_fullscreen(bool enable);
void toggle_double_buffering(bool enable);
void update_viewport();
void reshape(int w, int h);
void init_opengl();
void set_resize_mode(Resize_mode mode);
void set_gamma(double gamma);
void toggle_vsync(bool enable);
void init_rgb_pal();
Rgb24 pal8_to_rgb24_by_file(const Pal8 x);
Vector<float> load_ogl_palette(cr<Str> fname);
void set_palette(cr<Str> fname);
void check_max_gltex_sz();
void screen_tex_init();
void compile_program();
void compile_vertex_shader();
void compile_fragment_shader();
void init_screen_poly();
bool is_bad_wnd_sz();