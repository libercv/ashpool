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
    // TODO: Añadir contadores-> max ms por frame, min ms por frame,
    // media ms por frame, que avise del bvh (niveles, nodos, triángulos,
    // max_triángulos_por_leaf),
    window.pollEvents();
    timer.update();
    window.swapBuffers();
    input.move(timer.getDelta());
    updateStatistics();
  }

  void resetStatistics() {
    max_time = 0;
    min_time = 999999;
    total_time = 0;
    nr_frames = 0;
  }

  void setCamera(Camera *c) { input.setCamera(c); };
  bool exitRequested() { return window.shouldClose(); }
  void printStatistics() {
    nr_frames -= 1;
    std::cout << "Average FPS: "
              << (double)nr_frames * 1000 / (double)total_time << "\n";
    std::cout << "Nr of frames: " << nr_frames << "\n";
    std::cout << "Min time: " << min_time << " (ms)\n";
    std::cout << "Max time: " << max_time << " (ms)\n";
  }

private:
  Window window;
  Input input;
  Timer timer;
  unsigned int max_time = 0;
  unsigned int min_time = 999999;
  unsigned int total_time = 0;
  unsigned int nr_frames = 0;

  void updateStatistics() {
    if (nr_frames++ == 0)
      return;

    double delta = timer.getDelta() * 1000; // ms
    max_time = max_time > delta ? max_time : (unsigned int)delta;
    min_time = min_time < delta ? min_time : (unsigned int)delta;
    total_time += delta;
  }
};
#endif // SYSTEM_H
