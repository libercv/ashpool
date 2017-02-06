#include "cl_init.hpp"

#define GLFW_EXPOSE_NATIVE_GLX
#define GLFW_EXPOSE_NATIVE_X11

#include <fstream>
#include <iostream>
#include <sstream>

#include "cl_device.hpp"
#include <CL/cl_gl.h>
#include <CL/cl_gl_ext.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <cstring>
#include <string>

typedef CL_API_ENTRY cl_int(CL_API_CALL *clGetGLContextInfoKHR_fn)(
    const cl_context_properties *properties, cl_gl_context_info param_name,
    size_t param_value_size, void *param_value, size_t *param_value_size_ret);

// Rename references to this dynamically linked function to avoid
// collision with static link version
#define clGetGLContextInfoKHR clGetGLContextInfoKHR_proc
static clGetGLContextInfoKHR_fn clGetGLContextInfoKHR;

CL_Init::CL_Init() {
  auto plat_ids = CL_Platform::get_platforms_ids();
  if (plat_ids.size() == 0) {
    std::cout << "No OpenCL platforms detected\n";
    return;
  }

  // cl_platform_id plat_id=plat_ids[0];
  for (cl_platform_id &plat_id : plat_ids) {
    std::cout << "Detecting devices on platform " << plat_id << std::endl;
#ifdef linux
    cl_context_properties properties[] = {
        CL_GL_CONTEXT_KHR,
        (cl_context_properties)glfwGetGLXContext(glfwGetCurrentContext()),
        CL_GLX_DISPLAY_KHR,
        (cl_context_properties)glfwGetX11Display(),
        CL_CONTEXT_PLATFORM,
        (cl_context_properties)plat_id,
        0};
#elif defined _WIN32
    cl_context_properties properties[] = {
        CL_GL_CONTEXT_KHR,
        (cl_context_properties)wglGetCurrentContext(),
        CL_WGL_HDC_KHR,
        (cl_context_properties)wglGetCurrentDC(),
        CL_CONTEXT_PLATFORM,
        (cl_context_properties)plat_id,
        0};
#elif defined TARGET_OS_MAC
    CGLContextObj glContext = CGLGetCurrentContext();
    CGLShareGroupObj shareGroup = CGLGetShareGroup(glContext);
    cl_context_properties properties[] = {
        CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE,
        (cl_context_properties)shareGroup, 0};
#endif
    if (!clGetGLContextInfoKHR) {
      clGetGLContextInfoKHR =
          (clGetGLContextInfoKHR_fn)clGetExtensionFunctionAddressForPlatform(
              plat_id, "clGetGLContextInfoKHR");
      if (!clGetGLContextInfoKHR) {
        std::cout << "Failed to query proc address for clGetGLContextInfoKHR";
        continue;
      }
    }

    cl_device_id cl_dev;
    size_t size;
    cl_int result =
        clGetGLContextInfoKHR(properties, CL_CURRENT_DEVICE_FOR_GL_CONTEXT_KHR,
                              sizeof(cl_device_id), &cl_dev, &size);
    if (result != CL_SUCCESS) {
      std::cout << "Failed to obtain OpenCL Device from GL Device\n";
      continue;
    }
    device = CL_Device(cl_dev);
    cl_int status;
    ctxt = clCreateContext(properties, 1, &device.getId(), nullptr, nullptr,
                           &status);
    if (status != CL_SUCCESS) {
      std::cout << "Error creating OpenCL Context\n";
      continue;
    } else {
      std::cout << "CL Context created\n";
    }
    cmdQueue =
        clCreateCommandQueueWithProperties(ctxt, device.getId(), 0, &status);
    if (status != CL_SUCCESS) {
      std::cout << "Error creating OpenCL command queue\n";
      continue;
    }
    return;
  }
}

cl_mem CL_Init::createFromGLBuffer(GLuint GLBuffer, cl_mem_flags mem_flags,
                                   const std::string &str) {
  cl_int err;
  cl_mem buffer = clCreateFromGLBuffer(ctxt, mem_flags, GLBuffer, &err);
  if (!buffer || err != CL_SUCCESS) {
    std::cout << "Error acquiring CL texture from from OpenGL " << str << "\n";
  }

  return buffer;
}

cl_mem CL_Init::createFromGLTexture(GLuint GLtexture, cl_mem_flags mem_flags,
                                    const std::string &str) {
  cl_int err;
  cl_mem texture =
      clCreateFromGLTexture(ctxt, mem_flags, GL_TEXTURE_2D, 0, GLtexture, &err);
  if (!texture || err != CL_SUCCESS) {
    std::cout << "Error acquiring CL texture from from OpenGL " << str << "\n";
    switch (err) {
    case CL_INVALID_CONTEXT:
      std::cout << "CL_INVALID_CONTEXT\n";
      break;
    case CL_INVALID_VALUE:
      std::cout << "CL_INVALID_VALUE\n";
      break;
    case CL_INVALID_MIP_LEVEL:
      std::cout << "CL_INVALID_MIP_LEVEL\n";
      break;
    case CL_INVALID_GL_OBJECT:
      std::cout << "CL_INVALID_GL_OBJECT\n";
      break;
    case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
      std::cout << "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR\n";
      break;
    case CL_INVALID_OPERATION:
      std::cout << "CL_INVALID_OPERATION\n";
      break;
    case CL_OUT_OF_RESOURCES:
      std::cout << "CL_OUT_OF_RESOURCES\n";
      break;
    case CL_OUT_OF_HOST_MEMORY:
      std::cout << "CL_OUT_OF_HOST_MEMORY\n";
      break;
    }
  }
  return texture;
}

void CL_Init::checkForCLGLSharing() {
  /*
    auto dev_ids = CL_Device::get_devices_ids(plat_id);
    if (dev_ids.size() == 0) {
      std::cout << "No OpenCL devices found\n";
      continue;
    }

    std::memset(extension_string, ' ', 1024);
    device = std::make_unique<CL_Device>(dev_ids[0]);
    platform = std::make_unique<CL_Platform>(plat_id);
    cl_int status =
        clGetDeviceInfo(dev_ids[0], CL_DEVICE_EXTENSIONS,
                        sizeof(extension_string), extension_string, nullptr);
    char *extStringStart = nullptr;
    extStringStart = strstr(extension_string, "cl_khr_gl_sharing");
    std::cout << extension_string << "\n";
    if (extStringStart != nullptr) {
      std::cout << "Platform does support cl_khr_gl_sharing\n";
    } else {
      std::cout << "Platform doesn't support cl_khr_gl_sharing\n";
    }
  */
}

CL_Init::~CL_Init() {
  clReleaseCommandQueue(cmdQueue);
  clReleaseContext(ctxt);
}

void CL_Init::loadProgram(const std::string &path) {
  std::string code = readFile("shaders/render.cl");
  size_t p_source_len = code.size();
  cl_int status;
  const char *data = code.data();
  program = clCreateProgramWithSource(ctxt, 1, &data, &p_source_len, &status);

  if (status != CL_SUCCESS) {
    std::cout << "Error creating program: " << path << "\n";
    return;
  }

  status =
      clBuildProgram(program, 1, &device.getId(), nullptr, nullptr, nullptr);
  if (status != CL_SUCCESS) {
    std::cout << "Error building program: " << path << "\n";

    size_t buildLogSize = 0;
    clGetProgramBuildInfo(program, device.getId(), CL_PROGRAM_BUILD_LOG,
                          buildLogSize, nullptr, &buildLogSize);

    auto buildLog = std::make_unique<char[]>(buildLogSize);

    clGetProgramBuildInfo(program, device.getId(), CL_PROGRAM_BUILD_LOG,
                          buildLogSize, buildLog.get(), nullptr);

    std::cout << buildLog.get() << std::endl;
    exit(-1);
  }

  status = clCreateKernelsInProgram(program, 1, &kernel, nullptr);
  if (status != CL_SUCCESS) {
    std::cout << "Unable to create kernel from program\n";
    return;
  }

  std::cout << "OpenCL kernel loaded:" << path << "\n";
}

void CL_Init::setKernelArg(cl_uint index, size_t size, const void *value) {
  clSetKernelArg(kernel, index, size, value);
}

void CL_Init::executeKernel(cl_event *event) {
  size_t globalWorkSize[2] = {1280, 720};
  cl_int status = clEnqueueNDRangeKernel(cmdQueue, kernel, 2, nullptr,
                                         globalWorkSize, nullptr, 0, 0, event);
  if (status != CL_SUCCESS) {
    std::cout << "Error running kernel\n";
  }
}

std::string CL_Init::readFile(const std::string &path) {
  std::string code{""};
  try {
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    file.open(path);
    std::stringstream fStream;
    fStream << file.rdbuf();
    file.close();
    code = fStream.str();
  } catch (std::ifstream::failure &e) {
    std::cerr << "ERROR::CL_INIT::FILE_NOT_SUCCESFULLY_READ\n";
    std::cerr << "File: " << path << std::endl;
    std::exit(1);
  }
  return code;
}
