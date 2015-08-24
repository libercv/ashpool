#include "scene.hpp"
#include <GL/glew.h>                          // for GLfloat, glClear, etc
#include <GLFW/glfw3.h>                       // for glfwGetTime
#include <math.h>                             // for cos, sin
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "camera.hpp"                         // for Camera
#include "shaderprogram.hpp"                         // for Shader
#include "modelloader.hpp"


Scene::Scene() :
	shader { "shaders/shader.vert", "shaders/shader.frag"},
	model { ModelLoader::loadModel("models/sponza/sponza.obj") } {
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
		//glm::mat4 modelMatrix=glm::mat4();
		//auto modelLoc=shader.getUniformLocation("model");
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

		clear();

		auto speed = 0.25f * glfwGetTime();
		glm::vec3 la(sin(speed), 0.5f, cos(speed));
		camera.lookAt(glm::vec3(0.0f, 0.5f, 0.0f), //Pos
				la); //lookat

		camera.applyMVP(&shader, model.getModelMatrix());
		camera.applyMV(&shader, model.getModelMatrix());

		model.Draw(shader);
	}

	Scene::~Scene() {
		//glDeleteVertexArrays(1, &VAO);
		//glDeleteBuffers(1, &VBO);
	}
