#ifndef SCENE_H
#define SCENE_H

#include <memory>

//class Shader;
#include "shader.h"
#include "camera.h"
//class Camera;

class Scene {
private:
	Shader shader;
	Camera camera;
	GLuint VAO, VBO;

public:
	Scene();
	~Scene();
	void render();
	void clear();
};

#endif //SCENE_H
