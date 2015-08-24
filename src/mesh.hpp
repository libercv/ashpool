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
		Mesh() : mat{}, VAO{0}, VBO{0}, EBO{0} { };
		Mesh(Mesh &&other) :
			vertices(std::move(other.vertices)),
			indices(std::move(other.indices)),
			textures(std::move(other.textures)),
			mat(std::move(other.mat)),
			VAO(other.VAO), VBO(other.VBO), EBO(other.EBO) { 
				other.VBO=0; other.EBO=0; other.VAO=0; };
		Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures, Material material);
		~Mesh();

		Mesh& operator=(const Mesh& other) = delete;
		Mesh(const Mesh &other) = delete;

		void Draw(const ShaderProgram& shader) const;

	private:
		std::vector<Vertex> vertices;
		std::vector<GLuint> indices;
		std::vector<Texture> textures;
		struct Material mat;
		GLuint VAO, VBO, EBO;

		void setupMesh();
};
#endif // MESH_H
