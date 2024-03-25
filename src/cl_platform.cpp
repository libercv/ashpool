/***************************************************
 * CL_Platform
 *
 * Obtains information about OpenCL platforms in the
 * system
 *
 * 2017 - Liberto Camús
 * **************************************************/
#include "cl_platform.hpp"
#include <iostream>

std::vector<cl_platform_id> CL_Platform::get_platforms_ids() {
  cl_uint n_pat;
  cl_int status = clGetPlatformIDs(0, nullptr, &n_pat);
  if (status != CL_SUCCESS) {
    std::cout << "Couldn't find any OpenCL platforms." << std::endl;
    return std::vector<cl_platform_id>();
  }

  std::cout << "Found " << n_pat << " OpenCL platforms." << std::endl;
  auto ids = std::vector<cl_platform_id>(n_pat);
  clGetPlatformIDs(n_pat, ids.data(), nullptr);

  return ids;
}

CL_Platform::CL_Platform(cl_platform_id id) : platform_id{id} {
  size_t arg_size;
  // PLATFORM ID
  std::cout << "Platform ID:" << id << std::endl;

  // PLATFORM NAME
  clGetPlatformInfo(platform_id, CL_PLATFORM_NAME, 0, nullptr, &arg_size);
  name.resize(arg_size);
  clGetPlatformInfo(platform_id, CL_PLATFORM_NAME, arg_size, &name.front(),
                    nullptr);
  std::cout << "OpenCL platform:" << name << std::endl;

  // VENDOR NAME
  clGetPlatformInfo(platform_id, CL_PLATFORM_VENDOR, 0, nullptr, &arg_size);
  vendor_name.resize(arg_size);
  clGetPlatformInfo(platform_id, CL_PLATFORM_VENDOR, arg_size,
                    &vendor_name.front(), nullptr);
  std::cout << "OpenCL vendor:" << vendor_name << std::endl;

  // PROFILE
  clGetPlatformInfo(platform_id, CL_PLATFORM_PROFILE, 0, nullptr, &arg_size);
  profile.resize(arg_size);
  clGetPlatformInfo(platform_id, CL_PLATFORM_PROFILE, arg_size,
                    &profile.front(), nullptr);
  std::cout << "OpenCL profile:" << profile << std::endl;

  // VERSION
  clGetPlatformInfo(platform_id, CL_PLATFORM_VERSION, 0, nullptr, &arg_size);
  version.resize(arg_size);
  clGetPlatformInfo(platform_id, CL_PLATFORM_VERSION, arg_size,
                    &version.front(), nullptr);
  std::cout << "OpenCL version:" << version << std::endl;
}
