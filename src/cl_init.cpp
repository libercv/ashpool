#include "cl_init.hpp"
#include <iostream>
#include "cl_device.hpp"

CL_Init::CL_Init() {
  auto plat_ids = CL_Platform::get_platforms_ids();
  for (cl_platform_id &plat_id : plat_ids) {
    m_platforms.emplace_back(plat_id);
    std::cout << "Detecting devices on platform " << plat_id << std::endl;
    auto dev_ids = CL_Device::get_devices_ids(plat_id);
    for (auto &dev_id : dev_ids)
      auto d = CL_Device(dev_id);
  }
}
