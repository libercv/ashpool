#ifndef CONFIG_H
#define CONFIG_H
#include <glm/glm.hpp>
#include <string>
#include <vector>

class Config {
public:
  // window
  static uint window_width;
  static uint window_height;
  static float window_ratio;

  // Shaders / kernels
  static std::string gbuffer_shader_vert;
  static std::string gbuffer_shader_frag;
  static std::string lighting_shader_vert;
  static std::string lighting_shader_frag;
  static std::string lighting_kernel;

  // Camera
  static glm::vec3 camera_position;
  static float camera_znear;
  static float camera_zfar;
  static float camera_pitch;
  static float camera_yaw;

  // Movement
  static float movement_speed;
  static float movement_zoom;
  static float movement_sensitivity;

  // Scene
  static std::vector<std::string> models;
  static std::vector<std::vector<float>> point_lights;

  // Options
  static bool option_shadows_enabled;
};

#endif // CONFIG_H
