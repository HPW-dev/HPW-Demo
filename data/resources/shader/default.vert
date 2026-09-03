#version 330 core
in vec4 pos;
out vec2 uv;

void main() {
  uv = pos.zw;
  gl_Position = vec4(pos.xy, 0.0, 1.0);
}
