#include "camera.h"
#include "shader.h"

#include <GL/glew.h>


Camera::Camera() : upVector{ glm::vec3(0.0f, 1.0f, 0.0f) },
		projMatrix { glm::perspective( 45.0f, (float)800/(float)600, 0.1f, 100.0f )} 	  {

	//direction = glm::normalize( position - targt );
	//right = glm::normalize(glm::cross(upVector, direction));
	//up = glm::cross(direction, right);
}

void Camera::setTarget(glm::vec3 targt) {
	target=targt;
}

void Camera::setPosition(glm::vec3 pos) {
	position=pos;
}

void Camera::applyProjectionMatrix(const Shader *shader) {
	auto loc=shader->getUniformLocation("projection");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(projMatrix));
}

void Camera::lookAt(glm::vec3 pos, glm::vec3 targt) {
	target=targt;
	position=pos;
	viewMatrix=glm::lookAt(position, target, upVector);
}
	

void Camera::applyViewMatrix( const Shader *shader) {
	auto loc=shader->getUniformLocation("view");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
}
