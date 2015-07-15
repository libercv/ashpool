#ifndef WINDOW_H
#define WINDOW_H

#include <iostream>
#include <memory>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

class Window {
public:
	Window();
	~Window();
	bool shouldClose();
	void pollEvents();
	void swapBuffers();

private:
	// Window dimensions
	const GLuint WIDTH = 800, HEIGHT = 600;
	GLFWwindow *window;

};

#endif //WINDOW_H
