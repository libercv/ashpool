#ifndef SCENE_H
#define SCENE_H

#include <memory>

//class Shader;
#include "shader.h"
#include "camera.h"
#include "model.h"
//class Camera;

class Scene {
private:
	Shader shader;
	Camera camera;
	Model model;

public:
	Scene();
	~Scene();
	void render();
	void clear();
};

#endif //SCENE_H
