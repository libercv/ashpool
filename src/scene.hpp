#ifndef SCENE_H
#define SCENE_H

#include "camera.hpp"  // for Camera
#include "model.hpp"   // for Model
#include "shader.hpp"  // for Shader

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
