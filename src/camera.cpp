#include "camera.h"
#include "shader.h"

#include <GL/glew.h>


Camera::Camera() : upVector{ glm::vec3(0.0f, 1.0f, 0.0f) },
		projMatrix { glm::perspective( 45.0f, (float)800/(float)600, 0.1f, 100.0f )} 	  {
}


void Camera::lookAt(glm::vec3 pos, glm::vec3 targt) {
	target=targt;
	position=pos;
	viewMatrix=glm::lookAt(position, target, upVector);
}
	

void Camera::applyViewMatrix(const Shader &shader) {
	auto loc=shader.getUniformLocation("view");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
}

void Camera::applyProjectionMatrix(const Shader &shader) {
	auto loc=shader.getUniformLocation("projection");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(projMatrix));
}
