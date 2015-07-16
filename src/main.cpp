#include "window.h"
#include "scene.h"


int main() {

	Window window;
	Scene scene;

	while (!window.shouldClose()) {
		window.pollEvents();
		scene.clear();
		scene.render();
		window.swapBuffers();
	}

	return 0;
}

