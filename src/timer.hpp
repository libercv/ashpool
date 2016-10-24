#ifndef TIMER_H
#define TIMER_H
#include <GL/glew.h>        // for GLfloat
#include <GLFW/glfw3.h>     // for glfwGetTime

class Timer {
public:
  Timer() : lastTime { glfwGetTime() } {}
  ~Timer() {}
  void update() {
    double currentTime = glfwGetTime();
    deltaTime = currentTime - lastTime;
  }
  double getDelta() const { return deltaTime; }

private:
  double lastTime, deltaTime;
};

#endif // TIMER_H

