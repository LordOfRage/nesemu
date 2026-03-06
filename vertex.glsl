#version 440 core

layout(location = 0) in vec2 pos;

out vec2 texture_coordinates;

void main() {
  gl_Position = vec4(pos.xy, 0, 1);
  texture_coordinates = vec2((pos.x + 1) / 2, (pos.y + 1) / 2); // mapping the ranges [-1, 1] to [0, 1] for use as UV coordinates
}
