cmake_minimum_required(VERSION 3.1)

project(ashpool)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -fno-omit-frame-pointer")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
                      ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/)
add_executable(ashpool main.cpp 
		camera.hpp camera.cpp
    input.hpp input.cpp
		shaderloader.cpp shaderloader.hpp 
		shaderprogram.cpp shaderprogram.hpp 
    shader.hpp
		window.cpp window.hpp
		model.cpp model.hpp
		mesh.cpp mesh.hpp
		modelloader.cpp modelloader.hpp
    deferredshader.cpp deferredshader.hpp
    hybridshader.cpp hybridshader.hpp
		texture.hpp
		vertex.hpp
		material.hpp
    timer.hpp
    system.hpp
    renderengine.hpp
    pointlight.hpp
		texturemanager.cpp texturemanager.hpp
		cl_init.cpp cl_init.hpp
		cl_platform.cpp cl_platform.hpp
		cl_device.cpp cl_device.hpp
    world.cpp world.hpp
		textureloader.cpp textureloader.hpp)
find_package(PkgConfig REQUIRED)
#find_package(SOIL  REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)
find_package(ASSIMP REQUIRED) 
find_package(FreeImage REQUIRED) 
find_package(OpenCL REQUIRED) 

include_directories( ${OPENGL_INCLUDE_DIRS}  
	${GLFW_INCLUDE_DIRS} 
	${GLEW_INCLUDE_DIRS} 
	${GLM_INCLUDE_DIRS} 
#	${SOIL_INCLUDE_DIRS}
	${ASSIMP_INCLUDE_DIRS}
	${FREEIMAGE_INCLUDE_DIRS}
	${OpenCL_INCLUDE_DIRS}
	) 


target_link_libraries(ashpool 
	${OPENGL_LIBRARIES} 
	${GLFW_LIBRARIES} 
	${GLEW_LIBRARY} 
	${GLM_LIBRARY}
#	${SOIL_LIBRARIES}
	${ASSIMP_LIBRARIES}
	${FREEIMAGE_LIBRARIES}
	${OpenCL_LIBRARIES}
	)
