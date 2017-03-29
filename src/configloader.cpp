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

// Scene
std::vector<std::string> Config::models;
std::vector<std::vector<float>> Config::point_lights;
float Config::ambient = 0.0f;

// Runtime Options
bool Config::option_shadows_enabled = true;
bool Config::option_normal_mapping_enabled = false;
bool Config::option_rendering_method_change_requested = false;
bool Config::option_no_capture_mouse = false;

// Rendering Method
Config::RENDERING_METHOD Config::rendering_method = Config::HYBRID;

ConfigLoader::ConfigLoader(const std::string &file) {
  std::cout << "Reading configuration file: " << file << "\n";
  std::vector<std::string> data = readFile(file);

  for (std::string &line : data) {
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
      setWindow(result);
    } else if (title.compare("model") == 0) {
      setModel(result);
    } else if (title.compare("gbuffer_shader") == 0) {
      setGBufferShader(result);
    } else if (title.compare("lighting_shader") == 0) {
      setLightingShader(result);
    } else if (title.compare("lighting_kernel") == 0) {
      setLightingKernel(result);
    } else if (title.compare("camera") == 0) {
      setCamera(result);
    } else if (title.compare("movement") == 0) {
      setMovement(result);
    } else if (title.compare("ambient") == 0) {
      setAmbientLightIntensity(result);
    } else if (title.compare("pointlight") == 0) {
      setPointLight(result);
    }
  }
}
void ConfigLoader::setWindow(const std::vector<std::string> &v) {
  Config::window_width = std::atoi(v.at(1).c_str());
  Config::window_height = std::atoi(v.at(2).c_str());
  Config::window_ratio =
      (float)Config::window_width / (float)Config::window_height;
}

void ConfigLoader::setModel(const std::vector<std::string> &v) {
  Config::models.push_back(v.at(1));
}

void ConfigLoader::setGBufferShader(const std::vector<std::string> &v) {
  Config::gbuffer_shader_frag = v.at(1) + ".frag";
  Config::gbuffer_shader_vert = v.at(1) + ".vert";
}

void ConfigLoader::setLightingShader(const std::vector<std::string> &v) {
  Config::lighting_shader_frag = v.at(1) + ".frag";
  Config::lighting_shader_vert = v.at(1) + ".vert";
}

void ConfigLoader::setLightingKernel(const std::vector<std::string> &v) {
  Config::lighting_kernel = v.at(1);
}

void ConfigLoader::setCamera(const std::vector<std::string> &v) {
  Config::camera_position = glm::vec3{(float)std::atof(v.at(1).c_str()),
                                      (float)std::atof(v.at(2).c_str()),
                                      (float)std::atof(v.at(3).c_str())};
  Config::camera_znear = (float)std::atof(v.at(4).c_str());
  Config::camera_zfar = (float)std::atof(v.at(5).c_str());
  Config::camera_pitch = (float)std::atof(v.at(6).c_str());
  Config::camera_yaw = (float)std::atof(v.at(7).c_str());
}

void ConfigLoader::setMovement(const std::vector<std::string> &v) {
  Config::movement_speed = (float)std::atof(v.at(1).c_str());
  Config::movement_zoom = (float)std::atof(v.at(2).c_str());
  Config::movement_sensitivity = (float)std::atof(v.at(3).c_str());
}

void ConfigLoader::setPointLight(const std::vector<std::string> &v) {
  std::vector<float> v_f;
  v_f.push_back((float)std::atof(v.at(1).c_str())); // pos_x
  v_f.push_back((float)std::atof(v.at(2).c_str())); // pos_y
  v_f.push_back((float)std::atof(v.at(3).c_str())); // pos_z
  v_f.push_back((float)std::atof(v.at(4).c_str())); // color_r
  v_f.push_back((float)std::atof(v.at(5).c_str())); // color_g
  v_f.push_back((float)std::atof(v.at(6).c_str())); // color_b
  v_f.push_back((float)std::atof(v.at(7).c_str())); // diffuse
  v_f.push_back((float)std::atof(v.at(8).c_str())); // linear
  v_f.push_back((float)std::atof(v.at(9).c_str())); // quadratic

  Config::point_lights.emplace_back(v_f);
}

void ConfigLoader::setAmbientLightIntensity(const std::vector<std::string> &v) {
  Config::ambient = (float)std::atof(v.at(1).c_str());
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
