/***************************************************
 * Config
 *
 * Configuration and scene descripton values
 *
 * 2017 - Liberto Camús
 * **************************************************/
#ifndef CONFIG_H
#define CONFIG_H
#include <glm/glm.hpp>
#include <string>
#include <vector>

class Config {
public:
  enum RENDERING_METHOD { HYBRID, DEFERRED, HYBRID_CPU };

  // window
  static unsigned int window_width;
  static unsigned int window_height;
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
  static float movement_fovy_step;
  static float movement_sensitivity;

  // Scene
  static std::vector<std::string> models;
  static std::vector<std::vector<float>> point_lights;
  static float ambient;

  // Options
  static bool option_shadows_enabled;
  static bool option_normal_mapping_enabled;
  static bool option_rendering_method_change_requested;
  static bool option_no_capture_mouse;
  static bool option_opencl_available;
  static bool option_statistics_requested;
  static bool option_reset_statistics_requested;

  // Rendering method
  static RENDERING_METHOD rendering_method;
};

#endif // CONFIG_H
