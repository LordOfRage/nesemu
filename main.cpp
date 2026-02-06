#include "glew.h"
#include "glfw3.h"
#include "CPU.hpp"
#include "PPU.hpp"
#include "ROM.hpp"
#include <cstdio>
#include <string>
#include <iostream>
#include "debug.hpp"
#include "renderer.hpp"

unsigned int CompileShader(GLenum type, const std::string &filename) {
  unsigned int id = glCreateShader(type);
  std::ifstream t(filename);
  std::string shader((std::istreambuf_iterator<char>(t)),
                 std::istreambuf_iterator<char>());
  const char *shaderptr = shader.data();

  glShaderSource(id, 1, &shaderptr, nullptr);
  glCompileShader(id);

  return id;
}

void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char *message, const void *userParam) {
  std::cout << message;
}

int main() {
  if (!glfwInit()) throw std::runtime_error("GLFW failed to initialise!");

  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

  GLFWwindow *window;

  window = glfwCreateWindow(256*5, 240*5, "NES emulator", NULL, NULL);
  if (!window) {
    glfwTerminate();
    throw std::runtime_error("GLFW failed to create a window!");
  }

  glfwMakeContextCurrent(window);

  if (glewInit() != GLEW_OK) throw std::runtime_error("Glew failed to initialise!");

  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); 
  glEnable(GL_DEPTH_TEST);
  glDebugMessageCallback(glDebugOutput, nullptr);
  glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

  ROM rom = ROM("../../smb.nes");
  PPU ppu(rom);
  CPU cpu(rom, ppu);
  Renderer renderer(window, ppu);
  renderer.debugrom = &rom;

  byte a, x, y, p, sp;
  word pc;


  float triangles[8] = {
    -1, -1,
    -1, 1,
    1, -1,
    1, 1
  };

  unsigned int indeces[6] = {
    0, 1, 3,
    0, 3, 2
  };

  unsigned int buffer;
  glGenBuffers(1, &buffer);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBufferData(GL_ARRAY_BUFFER, 8*sizeof(float), triangles, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), 0);

  unsigned int ibo;
  glGenBuffers(1, &ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*sizeof(unsigned int), indeces, GL_STATIC_DRAW);

  unsigned int program = glCreateProgram();
  unsigned int vs = CompileShader(GL_VERTEX_SHADER, "../../vertex.glsl");
  unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, "../../fragment.glsl");
  glAttachShader(program, vs);
  glAttachShader(program, fs);
  glLinkProgram(program);
  glValidateProgram(program);

  glDeleteShader(vs);
  glDeleteShader(fs);
  glUseProgram(program);

  glUniform1i(glGetUniformLocation(program, "tex"), 0);

  cpu.accumulator = 0x10;
  cpu.x = 0xff;
  cpu.y = 0;
  cpu.sp = 0xff;
  cpu.debug_cycles = 99347969 + 32100;

  const double fpsLimit = 1.0 / 60.0;
  double lastUpdateTime = 0;  // number of seconds since the last loop
  double lastFrameTime = 0;   // number of seconds since the last frame

  while (!glfwWindowShouldClose(window)) {
    renderer.UnbindTexture();
    while (!ppu.nmitrigger) {
      a = cpu.accumulator;
      x = cpu.x;
      y = cpu.y;
      sp = cpu.sp;
      // debug << "$" << to_hex(cpu.pc);
      cpu.Decode(cpu.FetchPC());
      
      // debug << "\n";
    }
    ppu.nmitrigger = false;
    renderer.InitTexture(renderer.GetPixelsAsTexture());
    renderer.BindTexture();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    glfwSwapBuffers(window);

    while (glfwGetTime() - lastFrameTime < fpsLimit) {}
    lastFrameTime = glfwGetTime();
    glfwPollEvents();

    byte inp = glfwGetKey(window, GLFW_KEY_D)
      + glfwGetKey(window, GLFW_KEY_A) * 2
      + glfwGetKey(window, GLFW_KEY_S) * 4
      + glfwGetKey(window, GLFW_KEY_W) * 8
      + glfwGetKey(window, GLFW_KEY_U) * 16
      + glfwGetKey(window, GLFW_KEY_SPACE) * 32
      + glfwGetKey(window, GLFW_KEY_J) * 64
      + glfwGetKey(window, GLFW_KEY_K) * 128;

  }


  debug.close();
}
