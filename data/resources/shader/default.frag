#version 330 core
uniform sampler2D main_tex;
uniform sampler1D pal_tex;
out vec4 result_color;
in vec2 uv;

void main() {
  float pal_index = texture(main_tex, uv).r;
  vec3 rgb = texture(pal_tex, pal_index).rgb;
  result_color = vec4(rgb, 1.0);
}
