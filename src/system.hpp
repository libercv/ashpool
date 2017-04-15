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
#include "clkernelmanager.hpp"
#include <memory>

class System {
public:
  std::unique_ptr<CLKernelManager> clmanager;
  
  System() : input{window.createInput()} {}

  void update() {
    window.pollEvents();
    timer.update();
    window.swapBuffers();
    input.move(timer.getDelta());
    updateStatistics();
    
    if (Config::option_statistics_requested){
      Config::option_statistics_requested=false;
      printStatistics();
    }
    if (Config::option_reset_statistics_requested) {
      resetStatistics();
      Config::option_reset_statistics_requested=false;
    }
    
    nr_frames++;
  }

  void initOpenCL() {
    clmanager=std::make_unique<CLKernelManager>();
    clmanager->loadKernelFromFile(Config::lighting_kernel);
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
    //nr_frames -= 1;
    std::cout << "Rendering method: ";
    switch (Config::rendering_method) {
      case Config::DEFERRED:
        std::cout << "Deferred OpenGL shader\n";        
        break;
      case Config::HYBRID:
        std::cout << "Hybrid OpenCL shader\n";        
        break;
      default:
        std::cout << "Hybrid CPU shader\n";        
        break;
    }
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
    if (nr_frames == 0)
      return;

    double delta = timer.getDelta() * 1000; // ms
    max_time = max_time > delta ? max_time : (unsigned int)delta;
    min_time = min_time < delta ? min_time : (unsigned int)delta;
    total_time += delta;
  }
};
#endif // SYSTEM_H
