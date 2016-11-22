#ifndef CL_INIT_H
#define CL_INIT_H

#include "CL/cl.h"
#include "cl_platform.hpp"
#include "cl_device.hpp"


class CL_Init {
private:
  std::unique_ptr<CL_Platform> platform;
  std::unique_ptr<CL_Device> device;
  cl_context ctxt;
  cl_command_queue cmdQueue;

public:
  CL_Init();
  ~CL_Init();
};

#endif // CL_INIT_H
