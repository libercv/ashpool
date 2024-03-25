cmake_minimum_required(VERSION 3.1)

project(ashpool)
set(CMAKE_CXX_STANDARD 14)

IF(WIN32)
  set(LIBS_PREFIX "C:/lab/libs/")
  set(glfw3_DIR "${LIBS_PREFIX}GLFW/lib/cmake/glfw3")
  set(GLM_ROOT_DIR "${LIBS_PREFIX}/glm")
  set(FREEIMAGE_ROOT_DIR "${LIBS_PREFIX}/FreeImage")
  set(OpenCL_LIBRARY
    "C:/Program Files (x86)/AMD APP SDK/3.0/lib/x86_64/OpenCL.lib")
  set(OpenCL_INCLUDE_DIR "C:/Program Files (x86)/AMD APP SDK/3.0/include")
  set(CMAKE_PREFIX_PATH "${LIBS_PREFIX}/GLEW" "${LIBS_PREFIX}/FreeImage" "${LIBS_PREFIX}/Assimp/lib/cmake/assimp-3.3")
  set(CMAKE_LIBRARY_PATH "${LIBS_PREFIX}/GLEW/lib/Release/x64/" "${LIBS_PREFIX}/FreeImage/Dist/x64")
ELSE()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra -pedantic -fno-omit-frame-pointer -fno-rtti -ggdb -Wconversion")
ENDIF()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
           "${LIBS_PREFIX}/cmake"
           "${CMAKE_SOURCE_DIR}/cmake/Modules"
           ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/
           "${CMAKE_CURRENT_SOURCE_DIR}/Modules")

add_executable(ashpool
    bbox.hpp
    bvh.hpp bvh.cpp
    bvhlinearnode.hpp
    bvhprimitiveinfo.hpp
    camera.hpp camera.cpp
    cl_device.cpp cl_device.hpp
    clkernelmanager.cpp clkernelmanager.hpp
    cl_platform.cpp cl_platform.hpp
    config.hpp
    configloader.cpp configloader.hpp
    deferredshader.cpp deferredshader.hpp
    hybridshader.cpp hybridshader.hpp
    hybridshadercpu.cpp hybridshadercpu.hpp
    input.hpp input.cpp
    main.cpp
    material.hpp
    mesh.cpp mesh.hpp
    model.cpp model.hpp
    modelloader.cpp modelloader.hpp
    pointlight.hpp
    renderengine.hpp
    renderenginecreator.hpp
    shaderloader.cpp shaderloader.hpp
    shaderprogram.cpp shaderprogram.hpp
    system.hpp
    texture.hpp
    textureloader.cpp textureloader.hpp
    texturemanager.cpp texturemanager.hpp
    timer.hpp
    triangle.hpp
    vertex.hpp
    window.cpp window.hpp
    world.cpp world.hpp
    )

find_package(ASSIMP REQUIRED)
find_package(glfw3 3.2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)
find_package(FreeImage REQUIRED)
find_package(OpenCL REQUIRED)

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

if(WIN32)
  SET(ASSIMP_LIBRERIA ${ASSIMP_LIBRARY_DIRS}/${ASSIMP_LIBRARIES})
ELSE()
   SET(ASSIMP_LIBRERIA ${ASSIMP_LIBRARIES})
endif()

include_directories( ${OPENGL_INCLUDE_DIRS}
  ${GLFW_INCLUDE_DIRS}
  ${GLEW_INCLUDE_DIRS}
  ${GLM_INCLUDE_DIRS}
  ${ASSIMP_INCLUDE_DIRS}
  ${FREEIMAGE_INCLUDE_PATH}
  ${OpenCL_INCLUDE_DIRS}
  )

target_link_libraries(ashpool
  ${OPENGL_LIBRARIES}
  ${OPENGL_gl_LIBRARY}
  glfw ${GLFW_LIBRARIES}
  GLEW::GLEW
  ${GLEW_LIBRARY}
  ${GLM_LIBRARY}
  ${ASSIMP_LIBRERIA}
  ${FREEIMAGE_LIBRARIES}
  ${OpenCL_LIBRARIES}
  )

