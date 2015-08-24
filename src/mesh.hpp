#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>                 // for GLuint
#include <algorithm>                 // for move
#include <glm/glm.hpp>
#include <string>                    // for string
#include <vector>                    // for vector
#include <iostream>
#include "material.hpp"
#include "texture.hpp"
#include "vertex.hpp"

class ShaderProgram; 

class Mesh {
	public:
	/*  Mesh Data  */
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	/*  Functions  */
	// Constructor
	Mesh() : VAO{0}, VBO{0}, EBO{0} { std::cout << "Mesh: default ctor" << std::endl; };
	Mesh(Mesh &&other) :
		vertices(std::move(other.vertices)),
		indices(std::move(other.indices)),
		textures(std::move(other.textures)),
		mat(std::move(other.mat)),
		VAO(other.VAO), VBO(other.VBO), EBO(other.EBO) { 
			std::cout << "Mesh: moving constructor " << vertices.size() << " vertices." << std::endl; 
			other.VBO=0; other.EBO=0; other.VAO=0; };
	
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures, Material material);
	~Mesh();
	
	Mesh& operator=(const Mesh& other) = delete;
	Mesh(const Mesh &other) = delete;

	// Render the mesh
	void Draw(const ShaderProgram& shader) const;

	private:
	/*  Render data  */
	struct Material mat;
	GLuint VAO, VBO, EBO;

	/*  Functions    */
	// Initializes all the buffer objects/arrays
	void setupMesh();
};
#endif // MESH_H
