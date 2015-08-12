#include "window.hpp"
#include "scene.hpp"


int main() {

	Window window;
	Scene scene;

	while (!window.shouldClose()) {
		window.pollEvents();
		window.swapBuffers();
		scene.clear();
		scene.render();
	}

	return 0;
}

