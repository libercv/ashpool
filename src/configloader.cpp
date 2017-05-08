/***************************************************
 * ConfigLoader
 *
 * Initialization of "Config.hpp" values. Reads and
 * parses configuration files.
 *
 * 2017 - Liberto Camús
 * **************************************************/

#include "configloader.hpp"
#include "config.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

// Window
uint Config::window_width = 0;
uint Config::window_height = 0;
float Config::window_ratio = 0.0f;

// Shaders / kernels
std::string Config::gbuffer_shader_vert = "";
std::string Config::gbuffer_shader_frag = "";
std::string Config::lighting_shader_vert = "";
std::string Config::lighting_shader_frag = "";
std::string Config::lighting_kernel = "";

// Camera
glm::vec3 Config::camera_position = glm::vec3{0.0f, 0.0f, 0.0f};
float Config::camera_znear = 0.0f;
float Config::camera_zfar = 0.0f;
float Config::camera_pitch = 0.0f;
float Config::camera_yaw = 0.0f;

// Movement
float Config::movement_speed = 0.0f;
float Config::movement_zoom = 0.0f;
float Config::movement_sensitivity = 0.0f;
float Config::movement_fovy_step = 3.0f;

// Scene
std::vector<std::string> Config::models;
std::vector<std::vector<float>> Config::point_lights;
float Config::ambient = 0.0f;

// Runtime Options
bool Config::option_shadows_enabled = true;
bool Config::option_normal_mapping_enabled = false;
bool Config::option_rendering_method_change_requested = false;
bool Config::option_no_capture_mouse = false;
bool Config::option_opencl_available = true;
bool Config::option_statistics_requested = false;
bool Config::option_reset_statistics_requested = false;

// Rendering Method
Config::RENDERING_METHOD Config::rendering_method = Config::HYBRID;

ConfigLoader::ConfigLoader(const std::string &file) {
  std::cout << "Reading configuration file: " << file << "\n";
  std::vector<std::string> data = readFile(file);

  unsigned int line_nr = 0;
  bool file_ok = true;
  for (std::string &line : data) {
    ++line_nr;
    if (line.size() < 2)
      continue;
    if (line[0] == '/' && line[1] == '/')
      continue;

    std::vector<std::string> result;
    std::istringstream iss(line);
    for (std::string t; iss >> t;)
      result.push_back(t);

    if (result.size() == 0)
      continue;
    std::string title = result.at(0);

    if (title.compare("window") == 0) {
      file_ok = setWindow(result);
    } else if (title.compare("model") == 0) {
      file_ok = setModel(result);
    } else if (title.compare("gbuffer_shader") == 0) {
      file_ok = setGBufferShader(result);
    } else if (title.compare("lighting_shader") == 0) {
      file_ok = setLightingShader(result);
    } else if (title.compare("lighting_kernel") == 0) {
      file_ok = setLightingKernel(result);
    } else if (title.compare("camera") == 0) {
      file_ok = setCamera(result);
    } else if (title.compare("movement") == 0) {
      file_ok = setMovement(result);
    } else if (title.compare("ambient") == 0) {
      file_ok = setAmbientLightIntensity(result);
    } else if (title.compare("pointlight") == 0) {
      file_ok = setPointLight(result);
    }
    if (!file_ok) {
      std::cout << "Error reading configuration file " << file << "\n";
      std::cout << " in line " << line_nr << "\n";
      exit(1);
    }
  }
}
bool ConfigLoader::setWindow(const std::vector<std::string> &v) {
  if (v.size() < 3) {
    std::cout << "Too few arguments for window size. Expected \n";
    std::cout << "Window <width> <height> \n";
    return false;
  }
  Config::window_width = std::atoi(v.at(1).c_str());
  Config::window_height = std::atoi(v.at(2).c_str());
  Config::window_ratio =
      (float)Config::window_width / (float)Config::window_height;

  return true;
}

bool ConfigLoader::setModel(const std::vector<std::string> &v) {
  if (v.size() < 2) {
    std::cout << "Too few arguments for Model number. Expected \n";
    std::cout << "model <path> \n";
    return false;
  }
  Config::models.push_back(v.at(1));
  return true;
}

bool ConfigLoader::setGBufferShader(const std::vector<std::string> &v) {
  if (v.size() < 2) {
    std::cout << "Too few arguments for GBuffer shader name. Expected \n";
    std::cout << "gbuffer_shader <name> \n";
    return false;
  }
  Config::gbuffer_shader_frag = v.at(1) + ".frag";
  Config::gbuffer_shader_vert = v.at(1) + ".vert";
  return true;
}

bool ConfigLoader::setLightingShader(const std::vector<std::string> &v) {
  if (v.size() < 2) {
    std::cout << "Too few arguments for lighting shader name. Expected \n";
    std::cout << "lighting_shader <name> \n";
    return false;
  }
  Config::lighting_shader_frag = v.at(1) + ".frag";
  Config::lighting_shader_vert = v.at(1) + ".vert";
  return true;
}

bool ConfigLoader::setLightingKernel(const std::vector<std::string> &v) {
  if (v.size() < 2) {
    std::cout << "Too few arguments for lighting kernel name. Expected \n";
    std::cout << "lighting_kernel <name> \n";
    return false;
  }
  Config::lighting_kernel = v.at(1);
  return true;
}

bool ConfigLoader::setCamera(const std::vector<std::string> &v) {
  enum pos : size_t {
    pos_x = 1, pos_y, pos_z, znear, zfar, pitch, yaw, pos_end };
  if (v.size() < pos_end) {
    std::cout << "Too few arguments for camera parameters. Expected \n";
    std::cout
        << "camera <pos_x> <pos_y> <pos_z> <znear> <zfar> <pitch> <yaw> \n";
    return false;
  }
  Config::camera_position = glm::vec3{(float)std::atof(v.at(pos_x).c_str()),
                                      (float)std::atof(v.at(pos_y).c_str()),
                                      (float)std::atof(v.at(pos_z).c_str())};
  Config::camera_znear = (float)std::atof(v.at(znear).c_str());
  Config::camera_zfar = (float)std::atof(v.at(zfar).c_str());
  Config::camera_pitch = (float)std::atof(v.at(pitch).c_str());
  Config::camera_yaw = (float)std::atof(v.at(yaw).c_str());
  return true;
}

bool ConfigLoader::setMovement(const std::vector<std::string> &v) {
  enum pos : size_t { speed = 1, zoom, sensitivity, pos_end };
  if (v.size() < pos_end) {
    std::cout << "Too few arguments for movement parameters. Expected \n";
    std::cout << "movement <speed> <zoom> <sensitivity>\n";
    return false;
  }

  Config::movement_speed = (float)std::atof(v.at(speed).c_str());
  Config::movement_zoom = (float)std::atof(v.at(zoom).c_str());
  Config::movement_sensitivity = (float)std::atof(v.at(sensitivity).c_str());
  return true;
}

bool ConfigLoader::setPointLight(const std::vector<std::string> &v) {
  enum pos : size_t {
    pos_x = 1, pos_y, pos_z, color_r, color_g, color_b, intensity, linear,
    quadratic, pos_end };
  if (v.size() < pos_end) {
    std::cout << "Too few arguments for point light parameters. Expected \n";
    std::cout << "pointlight <pos_x> <pos_y> <pos_z> <color_r> <color_g> "
              << "<color_b> <intensity> <linear> <quadratic>\n";
    return false;
  }

  std::vector<float> v_f;
  v_f.push_back((float)std::atof(v.at(pos_x).c_str()));     // pos_x
  v_f.push_back((float)std::atof(v.at(pos_y).c_str()));     // pos_y
  v_f.push_back((float)std::atof(v.at(pos_z).c_str()));     // pos_z
  v_f.push_back((float)std::atof(v.at(color_r).c_str()));   // color_r
  v_f.push_back((float)std::atof(v.at(color_g).c_str()));   // color_g
  v_f.push_back((float)std::atof(v.at(color_b).c_str()));   // color_b
  v_f.push_back((float)std::atof(v.at(intensity).c_str())); // intensity
  v_f.push_back((float)std::atof(v.at(linear).c_str()));    // linear
  v_f.push_back((float)std::atof(v.at(quadratic).c_str())); // quadratic

  Config::point_lights.emplace_back(v_f);
  return true;
}

bool ConfigLoader::setAmbientLightIntensity(const std::vector<std::string> &v) {
  if (v.size() < 2) {
    std::cout << "Too few arguments for ambient value. Expected \n";
    std::cout << "ambient <number> \n";
    return false;
  }
  Config::ambient = (float)std::atof(v.at(1).c_str());
  return true;
}

std::vector<std::string> ConfigLoader::readFile(const std::string &path) {
  std::vector<std::string> res;
  std::string line;

  std::ifstream fich(path);
  if (!fich) {
    std::cout << "Error opening file:" << path << "\n";
    exit(-1);
  }
  while (std::getline(fich, line))
    res.push_back(line);

  return res;
}

ConfigLoader::~ConfigLoader() {}
