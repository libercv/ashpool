#ifndef CL_DEVICE_HPP
#define CL_DEVICE_HPP
#include <memory>
#include <vector>

#include "CL/cl.h"

class CL_Device {
  std::string name;
  cl_device_id device_id;

public:
  explicit CL_Device(cl_device_id);
  static std::vector<cl_device_id> get_devices_ids(cl_platform_id platform_id);
};

#endif // CL_DEVICE_HPP
