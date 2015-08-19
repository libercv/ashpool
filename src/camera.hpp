#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp> 

class Shader;  

class Camera {
private:
	glm::vec3 position;
	glm::vec3 target;
	//glm::vec3 direction;
	//glm::vec3 right;
	glm::vec3 upVector;

	glm::mat4 viewMatrix;
	glm::mat4 projMatrix;

	
public:
	Camera();
	void setTarget(glm::vec3 targt);
	void setPosition(glm::vec3 pos);
	void lookAt(glm::vec3 pos, glm::vec3 targt);
	void applyProjectionMatrix(const Shader*);
	void applyViewMatrix(const Shader *);
	void applyMVP(const Shader *, const glm::mat4 *modelMatrix);
	void applyMV(const Shader *, const glm::mat4 *modelMatrix);
};

#endif //CAMERA_H
