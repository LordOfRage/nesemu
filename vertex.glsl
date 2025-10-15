#version 440 core

layout(location = 0) in vec2 pos;

out vec2 txco;

void main() {
  gl_Position = vec4(pos.xy, 0, 1);
  txco = vec2((pos.x + 1) / 2, (pos.y + 1) / 2);
}
