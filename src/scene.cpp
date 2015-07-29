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
	shader { "shaders/shader.vert", "shaders/shader.frag"} {

	// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat vertices[] = {
		// Positions         // Colors
		0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // Bottom Right
		-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // Bottom Left
		0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // Top 
	};

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 
			6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 
			6 * sizeof(GLfloat), 
			(GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Unbind VAO
	glBindVertexArray(0); 

	// Camera
	camera.lookAt(glm::vec3(2.0f, 2.0f, -3.0f),
			glm::vec3(0.0f, 0.0f, 4.0f));
}

void Scene::clear() {
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}
	
void Scene::render() {
	// Model - View - Projection
	glm::mat4 model;
	model = glm::rotate(model, -55.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	auto modelLoc=shader.getUniformLocation("model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	camera.applyProjectionMatrix(&shader);
	camera.applyViewMatrix(&shader);

	shader.use();

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);
}

Scene::~Scene() { 
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

