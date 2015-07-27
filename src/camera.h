#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader;  

class Camera {
private:
	glm::vec3 pos;
	glm::vec3 target;
	glm::vec3 direction;
	glm::vec3 right;
	glm::vec3 up;

	glm::mat4 viewMatrix;
	glm::mat4 projMatrix;
	
	const Shader* shader;

	
public:
	Camera(glm::vec3 position, glm::vec3 targt, 
		const Shader *sha);
	void applyProjectionMatrix();
	void applyViewMatrix();
};

#endif //CAMERA_H
