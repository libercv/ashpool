#ifndef CONFIGLOADER_H
#define CONFIGLOADER_H
#include <string>
#include <vector>

class ConfigLoader {
public:
  ConfigLoader(const std::string &file);
  ~ConfigLoader();

private:
  void setWindow(const std::vector<std::string> &);
  void setModel(const std::vector<std::string> &);
  void setGBufferShader(const std::vector<std::string> &);
  void setLightingShader(const std::vector<std::string> &);
  void setLightingKernel(const std::vector<std::string> &);
  void setCamera(const std::vector<std::string> &);
  void setMovement(const std::vector<std::string> &);
  void setPointLight(const std::vector<std::string> &);

  std::vector<std::string> readFile(const std::string &path);
};

#endif // CONFIGLOADER_H
