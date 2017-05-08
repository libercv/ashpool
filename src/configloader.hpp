/***************************************************
 * ConfigLoader
 *
 * Initialization of "Config.hpp" values. Reads and
 * parses configuration files.
 *
 * 2017 - Liberto Camús
 * **************************************************/

#ifndef CONFIGLOADER_H
#define CONFIGLOADER_H
#include <string>
#include <vector>

class ConfigLoader {
public:
  ConfigLoader(const std::string &file);
  ~ConfigLoader();

  enum CONFIG_LOADER_POINT_LIGHT {

  };

private:
  bool setWindow(const std::vector<std::string> &);
  bool setModel(const std::vector<std::string> &);
  bool setGBufferShader(const std::vector<std::string> &);
  bool setLightingShader(const std::vector<std::string> &);
  bool setLightingKernel(const std::vector<std::string> &);
  bool setCamera(const std::vector<std::string> &);
  bool setMovement(const std::vector<std::string> &);
  bool setPointLight(const std::vector<std::string> &);
  bool setAmbientLightIntensity(const std::vector<std::string> &);

  std::vector<std::string> readFile(const std::string &path);
};

#endif // CONFIGLOADER_H
