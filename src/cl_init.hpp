#ifndef CL_INIT_H
#define CL_INIT_H

#include "CL/cl.h"
#include "cl_platform.hpp"

class CL_Init {
private:
  std::vector<CL_Platform> m_platforms;

public:
  CL_Init();
};

#endif // CL_INIT_H
