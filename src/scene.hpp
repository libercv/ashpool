#ifndef SCENE_H
#define SCENE_H

#include <memory>
#include "shaderprogram.hpp"

class Model;
class Camera;

class Scene {
private:
	ShaderProgram shader;
	std::unique_ptr<Camera> camera;
	std::unique_ptr<Model> model;

public:
	Scene();
	~Scene();
	void render();
	void clear();
};

#endif //SCENE_H
