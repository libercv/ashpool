#ifndef SCENE_H
#define SCENE_H

#include <memory>

class Shader;

class Scene {
private:
	std::unique_ptr<Shader> shader;
	GLuint VAO, VBO;

public:
	Scene();
	~Scene();
	void init();
	void render();
	void clear();
};

#endif //SCENE_H
