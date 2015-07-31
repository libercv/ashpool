#include <iostream>
#include <memory>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "shader.h"
#include "scene.h"

Scene::Scene() : 
	shader { "shaders/shader.vert", "shaders/shader.frag"},
	nanosuit {"models/kolumny/kolumny.obj"} {
	
	// Camera
	camera.lookAt(glm::vec3(1.0f, 1.0f, -4.0f),
			glm::vec3(0.0f, 0.0f, 4.0f));

}

void Scene::clear() {
	glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
	
void Scene::render() {
	// Model - View - Projection
	glm::mat4 model=glm::mat4();
	//model = glm::rotate(model, 3.14f, glm::vec3(0.0f, 1.0f, 0.0f));
	//model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); 
	//model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	
	auto modelLoc=shader.getUniformLocation("model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	camera.applyProjectionMatrix(&shader);
	camera.applyViewMatrix(&shader);
	
	nanosuit.Draw(shader);
	shader.use();
}

Scene::~Scene() { 
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

