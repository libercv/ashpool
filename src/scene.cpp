#include "scene.hpp"
#include <GL/glew.h>                          // for GLfloat, glClear, etc
#include <GLFW/glfw3.h>                       // for glfwGetTime
#include <math.h>                             // for cos, sin
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "camera.hpp"                         // for Camera
#include "shader.hpp"                         // for Shader


Scene::Scene() :
	shader { "shaders/shader.vert", "shaders/shader.frag"},
	model {"models/sponza/sponza.obj" } {
	//model {"models/sibenik/sibenik.obj" } {

	// Camera
	// sibenik cerca
	//camera.lookAt(glm::vec3(-13.0f, -12.0f, -0.0f), //Pos
	//		glm::vec3(20.0f, -2.0f, 0.0f)); //lookat
	// sibenik lejos
	//camera.lookAt(glm::vec3(-19.0f, -12.0f, -0.0f), //Pos
	//		glm::vec3(20.0f, -17.0f, 0.0f)); //lookat

	shader.use();

	glm::mat4 modelMatrix=glm::mat4();
	auto modelLoc=shader.getUniformLocation("model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));




	camera.lookAt(glm::vec3(0.0f, 0.5f, 0.0f), //Pos
			glm::vec3(5.0f, 1.5f, 0.0f)); //lookat
}

void Scene::clear() {
	glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Scene::render() {
	// Model - View - Projection
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//	glm::mat4 modelMatrix=glm::mat4();
//	auto modelLoc=shader.getUniformLocation("model");
//	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	
	glm::vec3 la(3.0f*sin((GLfloat)glfwGetTime()*0.25f), 0.5f, 3.0f*cos((GLfloat)glfwGetTime()*0.25f));
	camera.lookAt(glm::vec3(0.0f, 0.5f, 0.0f), //Pos
			la); //lookat


	camera.applyProjectionMatrix(&shader);
	camera.applyViewMatrix(&shader);

	model.Draw(shader);
	shader.use();
}

Scene::~Scene() {
	//glDeleteVertexArrays(1, &VAO);
	//glDeleteBuffers(1, &VBO);
}
