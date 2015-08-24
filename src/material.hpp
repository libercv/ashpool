#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/glm.hpp>


struct Material{
	glm::vec4 diffuse {0.8f, 0.8f, 0.8f, 1.0f};
	glm::vec4 ambient {0.2f, 0.2f, 0.2f, 1.0f};
	glm::vec4 specular {0.0f, 0.0f, 0.0f, 1.0f};
	glm::vec4 emissive {0.0f, 0.0f, 0.0f, 1.0f};

	float shininess;
	int texCount {0};
};



#endif // MATERIAL_H
