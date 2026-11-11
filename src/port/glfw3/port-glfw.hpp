#pragma once

void glfw3_init(int argc, char** argv);
[[nodiscard]] bool glfw3_is_ran();
void glfw3_update();
void glfw3_render_canvas();
void glfw3_sync();
void glfw3_finalize();
