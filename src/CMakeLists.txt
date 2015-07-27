cmake_minimum_required(VERSION 3.1)

project(ashpool)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")
add_executable(ashpool main.cpp 
		camera.h camera.cpp
		shader.cpp shader.h 
		window.cpp window.h
		scene.cpp scene.h)
find_package(PkgConfig REQUIRED)
find_package(SOIL  REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)
find_package(ASSIMP REQUIRED) 

include_directories( ${OPENGL_INCLUDE_DIRS}  
	${GLFW_INCLUDE_DIRS} 
	${GLEW_INCLUDE_DIRS} 
	${GLM_INCLUDE_DIRS} 
	${SOIL_INCLUDE_DIRS}
	${ASSIMP_INCLUDE_DIRS}
	) 


target_link_libraries(ashpool 
	${OPENGL_LIBRARIES} 
	${GLFW_LIBRARIES} 
	${GLEW_LIBRARY} 
	${GLM_LIBRARY}
	${SOIL_LIBRARIES}
	${ASSIMP_LIBRARIES}
	)
