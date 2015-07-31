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
	model  {"models/kolumny/kolumny.obj"} {
	
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
	auto modelMatrix { glm::mat4() };
	auto modelLoc=shader.getUniformLocation("model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	camera.applyProjectionMatrix(shader);
	camera.applyViewMatrix(shader);
	model.Draw(shader);
	shader.use();
}

Scene::~Scene() { 
	//glDeleteVertexArrays(1, &VAO);
	//glDeleteBuffers(1, &VBO);
}

