#include "camera.h"
#include "shader.h"

#include <GL/glew.h>


Camera::Camera(glm::vec3 position, glm::vec3 targt, const Shader *sha) : 
	pos{position}, target{targt}, shader{sha} {

	glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);

	direction = glm::normalize( position - targt );
	right = glm::normalize(glm::cross(upVector, direction));
	up = glm::cross(direction, right);

	viewMatrix=glm::lookAt(position, target, upVector);
	projMatrix= glm::perspective( 45.0f, (float)800/(float)600, 0.1f, 100.0f );
}



void Camera::applyProjectionMatrix() {
	auto loc=shader->getUniformLocation("projection");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(projMatrix));
}


void Camera::applyViewMatrix() {
	auto loc=shader->getUniformLocation("view");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
}
