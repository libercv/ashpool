cmake_minimum_required(VERSION 3.1)

project(ashpool)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra -pedantic -fno-omit-frame-pointer -fno-rtti -ggdb")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
                      ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/)
add_executable(ashpool 
    bbox.hpp
    bvh.hpp bvh.cpp
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
		vertex.hpp
		window.cpp window.hpp
    world.cpp world.hpp
		)
find_package(PkgConfig REQUIRED)
find_package (glfw3 3.2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
#find_package(glm REQUIRED)
find_package(ASSIMP REQUIRED) 
find_package(FreeImage REQUIRED) 
find_package(OpenCL REQUIRED) 

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

include_directories( ${OPENGL_INCLUDE_DIRS}  
	${GLFW_INCLUDE_DIRS} 
	${GLEW_INCLUDE_DIRS} 
#	${GLM_INCLUDE_DIRS} 
	${ASSIMP_INCLUDE_DIRS}
	${FREEIMAGE_INCLUDE_DIRS}
	${OpenCL_INCLUDE_DIRS}
	) 


target_link_libraries(ashpool 
	${OPENGL_LIBRARIES} 
  ${OPENGL_gl_LIBRARY}
	glfw ${GLFW_LIBRARIES} 
  GLEW::GLEW
#	${GLEW_LIBRARY} 
#	${GLM_LIBRARY}
	${ASSIMP_LIBRARIES}
	${FREEIMAGE_LIBRARIES}
	${OpenCL_LIBRARIES}
	)
