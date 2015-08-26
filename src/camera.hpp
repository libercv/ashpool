#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp> 

class ShaderProgram;  

class Camera {
private:
	const glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
	static constexpr float fovy {45.0f};
	static constexpr float zNear {0.1f};
	static constexpr float zFar {100.0f};
	
	glm::vec3 position;
	glm::vec3 target;
	glm::mat4 viewMatrix;
	glm::mat4 projMatrix;

	
public:
	Camera();
	void setTarget(glm::vec3 targt);
	void setPosition(glm::vec3 pos);
	void lookAt(glm::vec3 pos, glm::vec3 targt);
	void applyProjectionMatrix(const ShaderProgram*);
	void applyViewMatrix(const ShaderProgram *);
	void applyMVP(const ShaderProgram *, const glm::mat4 *modelMatrix);
	void applyMV(const ShaderProgram *, const glm::mat4 *modelMatrix);
};

#endif //CAMERA_H
