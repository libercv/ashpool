#ifndef VERTEX_H
#define VERTEX_H
#include <GL/glew.h>                 // for GLuint
#include <assimp/types.h>            // for aiString
#include <algorithm>                 // for move
#include <glm/glm.hpp>
#include <string>                    // for string
#include <vector>                    // for vector


class Vertex {
	public:
		Vertex() = default;
		Vertex(Vertex &&other) :
			Position(std::move(other.Position)),
			Normal(std::move(other.Normal)),
			TexCoords(std::move(other.TexCoords)) { };
		Vertex(glm::vec3 pos, glm::vec3 nor, glm::vec2 tex) :
			Position(std::move(pos)),
			Normal(std::move(nor)),
			TexCoords(std::move(tex)) { };
		Vertex& operator=(const Vertex& other) = default;
		Vertex(const Vertex& other) = default;

		// Position
		glm::vec3 Position;
		// Normal
		glm::vec3 Normal;
		// TexCoords
		glm::vec2 TexCoords;
};

#endif // VERTEX_H

