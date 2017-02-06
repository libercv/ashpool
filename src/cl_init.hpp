#ifndef CL_INIT_H
#define CL_INIT_H

#include "CL/cl.h"
#include "cl_device.hpp"
#include "cl_platform.hpp"
#include <GL/glew.h> // for GLuint
#include <string>

class CL_Init {
private:
  CL_Platform platform;
  CL_Device device;
  cl_context ctxt;
  cl_command_queue cmdQueue;
  cl_program program;
  cl_kernel kernel;
  void checkForCLGLSharing();
  std::string readFile(const std::string &path);

public:
  CL_Init();
  ~CL_Init();
  cl_context const &get_context() const { return ctxt; }
  cl_command_queue const &get_cmd_queue() const { return cmdQueue; }
  cl_mem createFromGLTexture(GLuint GLtexture, cl_mem_flags mem_flags,
                             const std::string &str);
  cl_mem createFromGLBuffer(GLuint GLBuffer, cl_mem_flags mem_flags,
                            const std::string &str);
  cl_mem createBuffer(size_t size, void *ptr) {
    return clCreateBuffer(ctxt, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, size,
                          ptr, nullptr);
  }

  void loadProgram(const std::string &path);
  void setKernelArg(cl_uint index, size_t size, const void *value);
  void executeKernel(cl_event *event);
};

#endif // CL_INIT_H
