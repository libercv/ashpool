#ifndef SCENE_H
#define SCENE_H

#include <memory>
#include "shaderprogram.hpp"

class Model;
class Camera;

class Scene {
private:
	GLuint gBuffer, gNormal, gAlbedo;
	GLuint gPosition, gColorSpec, gAlbedoSpec;
	GLuint rboDepth;
	ShaderProgram gBufferShader;
	ShaderProgram lightingPassShader;
	std::unique_ptr<Camera> camera;
	std::unique_ptr<Model> model;

public:
	Scene();
	~Scene();
	void render();
	void clear();
};

#endif //SCENE_H
