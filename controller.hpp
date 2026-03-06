#pragma once
#include <cstdint>
#include "glfw3.h"
#define byte uint8_t
#define word uint16_t
#define sbyte int8_t
#define sword int16_t

class Controller {
public:
  Controller(GLFWwindow*);
  ~Controller();

  void Poll();

  byte FetchMMIO(word);
  void WriteMMIO(word, byte);

private:
  byte inputs;
  bool strobing; // flag for whether the controller is being strobed or not
  
  bool ShiftBit();

  GLFWwindow *window;
};
