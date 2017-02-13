/***************************************************
 * CL_Platform
 *
 * Obtains information about OpenCL platforms in the
 * system
 *
 * 2017 - Liberto Camús
 * **************************************************/
#ifndef CL_PLATFORM_H
#define CL_PLATFORM_H
#include <memory>
#include <vector>

#include "CL/cl.h"

class CL_Platform {
private:
  cl_platform_id platform_id;
  std::string vendor_name;
  std::string name;
  std::string version;
  std::string profile;

public:
  CL_Platform(cl_platform_id id);
  CL_Platform() : platform_id(0) {}
  static std::vector<cl_platform_id> get_platforms_ids();
};

#endif // CL_PLATFORM_H
