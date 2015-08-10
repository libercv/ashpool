#ifndef MESH_H
#define MESH_H


// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
// GL Includes
#include <GL/glew.h> // Contains all the necessery OpenGL includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Shader;
class Model;

struct Texture {
    GLuint id;
    std::string type;
    aiString path;
};

struct Material{
	glm::vec4 diffuse {0.8f, 0.8f, 0.8f, 1.0f};
	glm::vec4 ambient {0.2f, 0.2f, 0.2f, 1.0f};
	glm::vec4 specular {0.0f, 0.0f, 0.0f, 1.0f};
	glm::vec4 emissive {0.0f, 0.0f, 0.0f, 1.0f};

	float shininess;
	int texCount {0};
};


class Mesh {

	class Vertex {
		public:
			Vertex(Vertex &&other) : 
				Position(std::move(other.Position)), 
				Normal(std::move(other.Normal)),
				TexCoords(std::move(other.TexCoords)) { };
			Vertex(glm::vec3 pos, glm::vec3 nor, glm::vec2 tex) :
				Position(std::move(pos)),
				Normal(std::move(nor)),
				TexCoords(std::move(tex)) { };
			//Vertex& operator=(const Vertex& other) = default;
			Vertex(const Vertex& other) = default;

			// Position
			glm::vec3 Position;
			// Normal
			glm::vec3 Normal;
			// TexCoords
			glm::vec2 TexCoords;
	};


	public:
	/*  Mesh Data  */
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	/*  Functions  */
	// Constructor
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures);
	Mesh(const aiMesh* mesh, const aiScene* scene, const std::string &directory); 
	~Mesh();

	// Render the mesh
	void Draw(const Shader& shader);

	private:
	/*  Render data  */
	GLuint VAO, VBO, EBO;
	struct Material mat;	

	void loadMaterial(aiMaterial *);
	/*  Functions    */
	// Initializes all the buffer objects/arrays
	void setupMesh();
};
#endif // MESH_H
