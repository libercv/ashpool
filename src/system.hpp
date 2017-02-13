/***************************************************
 * System
 *
 * Contains the timer, input and window objects
 *
 * 2017 - Liberto Camús
 * **************************************************/
#ifndef SYSTEM_H
#define SYSTEM_H

#include "input.hpp"
#include "timer.hpp"
#include "window.hpp"

class System {
public:
  System() : input{window.createInput()} {}

  void update() {
    window.pollEvents();
    timer.update();
    window.swapBuffers();
    input.move(timer.getDelta());
  }

  void setCamera(Camera *c) { input.setCamera(c); };
  bool exitRequested() { return window.shouldClose(); }

private:
  Window window;
  Input input;
  Timer timer;
};
#endif // SYSTEM_H
