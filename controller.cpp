#include "controller.hpp"
#include "glfw3.h"
#include <cstdio>

Controller::Controller(GLFWwindow *w) {
  window = w;
}

Controller::~Controller() {

}

void Controller::Poll() {
  inputs = glfwGetKey(window, GLFW_KEY_D)       // right
    + glfwGetKey(window, GLFW_KEY_A) * 2        // left
    + glfwGetKey(window, GLFW_KEY_S) * 4        // down
    + glfwGetKey(window, GLFW_KEY_W) * 8        // up
    + glfwGetKey(window, GLFW_KEY_U) * 16       // start
    + glfwGetKey(window, GLFW_KEY_SPACE) * 32   // select
    + glfwGetKey(window, GLFW_KEY_J) * 64       // b
    + glfwGetKey(window, GLFW_KEY_K) * 128;     // a
  printf("%d\n", inputs);
}

bool Controller::ShiftBit() {
  bool ret = inputs & 0x80; // most significant bit
  if (!strobing) inputs <<= 1;
  return ret;
}

byte Controller::FetchMMIO(word addr) {
  if (addr != 0x4016) return 0; // only supporting player 1 for now
  
  return ShiftBit();
}

void Controller::WriteMMIO(word addr, byte val) {
  if (addr != 0x4016) return; // only supporting player 1 for now
  
  strobing = val & 1; // set strobing flag to least significant bit
  if (strobing) Poll();
}
