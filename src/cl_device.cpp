#include "cl_device.hpp"
#include <iostream>

CL_Device::CL_Device(cl_device_id id) : device_id{id} {
  size_t arg_size;
  // DEVICE NAME
  clGetDeviceInfo(device_id, CL_DEVICE_NAME, 0, NULL, &arg_size);
  name.resize(arg_size);
  clGetDeviceInfo(device_id, CL_DEVICE_NAME, arg_size, &name[0], NULL);
  std::cout << "Device name:" << name << std::endl;
}

std::vector<cl_device_id>
CL_Device::get_devices_ids(cl_platform_id platform_id) {
  cl_uint n_dev;
  clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 0, nullptr, &n_dev);
  if (n_dev == 0) {
    std::cout << "No GPUs detected. Falling back to CPU" << std::endl;
    clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_CPU, 0, nullptr, &n_dev);
    auto ids = std::vector<cl_device_id>(n_dev);
    clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_CPU, n_dev, ids.data(), nullptr);
    return ids;
  } else {
    auto ids = std::vector<cl_device_id>(n_dev);
    clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, n_dev, ids.data(), nullptr);
    return ids;
  }
}
