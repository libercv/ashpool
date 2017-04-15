/***************************************************
 * CLKernelManager
 *
 * Encapsulates OpenCL initialization and functions
 * Also loads and compiles a kernel.
 *
 * 2017 - Liberto Camús
 * **************************************************/
#ifndef CL_KERNELMANAGER_H
#define CL_KERNELMANAGER_H

#include "CL/cl.h"
#include "cl_device.hpp"
#include "cl_platform.hpp"
#include <CL/cl_gl_ext.h>
#include <GL/glew.h> // for GLuint
#include <string>

class CLKernelManager {
private:
  CL_Platform platform;
  CL_Device device;
  cl_context ctxt;
  cl_command_queue cmdQueue;
  cl_program program;
  cl_kernel kernel;
  bool checkForCLGLSharing(cl_device_id dev_id);
  std::string readFile(const std::string &path);
  bool createSharedContext(cl_platform_id plat_id);

public:
  CLKernelManager();
  ~CLKernelManager();

  cl_mem createFromGLTexture(GLuint GLtexture, cl_mem_flags mem_flags,
                             const std::string &str);
  cl_mem createFromGLBuffer(GLuint GLBuffer, cl_mem_flags mem_flags,
                            const std::string &str);
  cl_mem createBufferReadOnly(size_t size, void *ptr) {
    return clCreateBuffer(ctxt, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, size,
                          ptr, nullptr);
  }
  cl_mem createBuffer(size_t size, void *ptr) {
    return clCreateBuffer(ctxt, CL_MEM_READ_WRITE, size,
                          ptr, nullptr);
  }

  void enqueueAcquireGLObjects(uint count, const cl_mem *cl_shared_objects) {
    clEnqueueAcquireGLObjects(cmdQueue, count, cl_shared_objects, 0, nullptr,
                              nullptr);
  }
  void enqueueReleaseGLObjects(uint count, const cl_mem *cl_shared_objects) {
    clEnqueueReleaseGLObjects(cmdQueue, count, cl_shared_objects, 0, nullptr,
                              nullptr);
  }
  void finish() { clFinish(cmdQueue); }

  void loadKernelFromFile(const std::string &path);
  void setKernelArg(cl_uint index, size_t size, const void *value);
  void executeKernel(cl_event *event, size_t third);
};

#endif // CL_KERNELMANAGER_H
