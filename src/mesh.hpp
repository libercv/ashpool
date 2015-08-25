#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>                 // for GLuint
#include <algorithm>                 // for move
#include <vector>                    // for vector
#include "material.hpp"
#include "texture.hpp"
#include "vertex.hpp"

class ShaderProgram; 

class Mesh {
	public:
		Mesh() : material{}, VAO{0}, VBO{0}, EBO{0} { };
		Mesh(Mesh &&other) :
			vertices(std::move(other.vertices)),
			indices(std::move(other.indices)),
			specular_textures(std::move(other.specular_textures)),
			diffuse_textures(std::move(other.diffuse_textures)),
			material(std::move(other.material)),
			VAO(other.VAO), VBO(other.VBO), EBO(other.EBO),
			indicesSize(other.indicesSize) { 
				other.VBO=0; other.EBO=0; other.VAO=0;
				other.indicesSize=0;};
		Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices,
				std::vector<Texture> specular_textures, std::vector<Texture> diffuse_textures,
				Material material);
		~Mesh();

		Mesh& operator=(const Mesh& other) = delete;
		Mesh(const Mesh &other) = delete;

		void refreshUniforms(const ShaderProgram &shader); 
		void draw(const ShaderProgram& shader) const;

	private:
		std::vector<Vertex> vertices;
		std::vector<GLuint> indices;
		std::vector<Texture> specular_textures;
		std::vector<Texture> diffuse_textures;
		struct Material material;
		GLuint VAO, VBO, EBO;
		GLuint indicesSize {0};

		void setupMesh();
};
#endif // MESH_H
