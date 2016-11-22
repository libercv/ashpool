#include "cl_init.hpp"

#define GLFW_EXPOSE_NATIVE_GLX
#define GLFW_EXPOSE_NATIVE_X11

#include <iostream>
#include "cl_device.hpp"
#include <CL/cl_gl.h>
#include <CL/cl_gl_ext.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <string>
#include <cstring>

typedef CL_API_ENTRY cl_int (CL_API_CALL *clGetGLContextInfoKHR_fn)(
    const cl_context_properties *properties,
    cl_gl_context_info param_name,
    size_t param_value_size,
    void *param_value,
    size_t *param_value_size_ret);

// Rename references to this dynamically linked function to avoid
// collision with static link version
#define clGetGLContextInfoKHR clGetGLContextInfoKHR_proc
static clGetGLContextInfoKHR_fn clGetGLContextInfoKHR;

CL_Init::CL_Init() : platform{nullptr}, device{nullptr} {
  auto plat_ids = CL_Platform::get_platforms_ids();
  if (plat_ids.size()==0) {
    std::cout << "No OpenCL platforms detected\n";
    return;
  }

 //cl_platform_id plat_id=plat_ids[0];
 for (cl_platform_id &plat_id : plat_ids) {
   std::cout << "Detecting devices on platform " << plat_id << std::endl;
    /*
    auto dev_ids = CL_Device::get_devices_ids(plat_id);
    if (dev_ids.size()==0)  {
      std::cout << "No OpenCL devices found\n";
      continue;
    }
    */
    /*
    std::memset(extension_string, ' ', 1024);
    device = std::make_unique<CL_Device>(dev_ids[0]);
    platform = std::make_unique<CL_Platform>(plat_id);
    cl_int status = clGetDeviceInfo( dev_ids[0],
              CL_DEVICE_EXTENSIONS ,
              sizeof(extension_string), 
              extension_string, 
              nullptr);
    char *extStringStart = nullptr;
    extStringStart = strstr(extension_string, "cl_khr_gl_sharing");
    std::cout << extension_string << "\n";
    if(extStringStart != nullptr){
      std::cout << "Platform does support cl_khr_gl_sharing\n";
    } else {
      std::cout << "Platform doesn't support cl_khr_gl_sharing\n";
    }
*/
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
        (cl_context_properties)platform,
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
    if (result!=CL_SUCCESS) {
      std::cout << "Failed to obtain OpenCL Device from GL Device\n";
      continue;
    }
    device = std::make_unique<CL_Device>(cl_dev);
    cl_int status;
    ctxt = clCreateContext(nullptr, 1, &device->getId(), nullptr, nullptr,
                           &status);
    if (status != CL_SUCCESS) {
      std::cout << "Error creating OpenCL Context\n";
    }
    cmdQueue =
        clCreateCommandQueueWithProperties(ctxt, device->getId(), 0, &status);
    if (status != CL_SUCCESS) {
      std::cout << "Error creating OpenCL command queue\n";
    }
    return;
  }
}

CL_Init::~CL_Init() {
  clReleaseCommandQueue(cmdQueue);
  clReleaseContext(ctxt);
}
