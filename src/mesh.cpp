#include "mesh.hpp"
#include <GL/glew.h>                 // for GLuint, glBindVertexArray, etc
#include <assimp/color4.h>           // for aiColor4D
#include <assimp/material.h>         // for aiGetMaterialColor, etc
#include <assimp/mesh.h>             // for aiMesh, aiFace
#include <assimp/scene.h>            // for aiScene
#include <assimp/vector3.h>          // for aiVector3t, aiVector3D
#include <ext/alloc_traits.h>        // for __alloc_traits<>::value_type
#include <stddef.h>                  // for offsetof
#include <assimp/material.inl>       // for aiMaterial::GetTextureCount
#include <iterator>                  // for back_insert_iterator, etc
#include <iostream>
#include <sstream>                   // for stringstream, basic_ostream
#include "shaderprogram.hpp"                // for Shader
#include "texturemanager.hpp"        // for TextureManager



Mesh::Mesh(std::vector<Vertex> Vertices,  std::vector<GLuint> Indices,
		std::vector<Texture> Specular_Textures, std::vector<Texture> Diffuse_Textures,
		Material mtl) :
	vertices {std::move(Vertices)}, 
	indices {std::move(Indices)}, 
	specular_textures {std::move(Specular_Textures)},
	diffuse_textures {std::move(Diffuse_Textures)},
	material {std::move(mtl)} {
		this->setupMesh();
	}

void Mesh::refreshUniforms(const ShaderProgram &shader) {
	auto diffuse_name = std::string("texture_diffuse");
	auto specular_name = std::string("texture_specular");

	GLuint diffuseNr = 1;
	for (auto &tex:diffuse_textures) {
		std::stringstream ss;
		ss << diffuseNr++;
		tex.uniformId=shader.getUniformLocation(diffuse_name + ss.str());
	}
	GLuint specularNr = 1;
	for (auto &tex:specular_textures) {
		std::stringstream ss;
		ss << specularNr++;
		tex.uniformId=shader.getUniformLocation(specular_name + ss.str());
	}
	material.shininess_uniform = shader.getUniformLocation("material_shininess");
	material.diffuse_uniform = shader.getUniformLocation("material_diffuse");
	material.ambient_uniform=shader.getUniformLocation("material_ambient");
	material.texCount_uniform=shader.getUniformLocation("material_texCount");
}

void  Mesh::draw(const ShaderProgram& shader) const {
	
	shader.use();
	
	// Bind textures
	int i=0;
	for (auto& tex:diffuse_textures) {
		glActiveTexture(GL_TEXTURE0 + i); 
		glUniform1i(tex.uniformId, i);
		glBindTexture(GL_TEXTURE_2D, tex.id);
		i=i+1;
	}
	for (auto& tex:specular_textures) {
		glActiveTexture(GL_TEXTURE0 + i); 
		glUniform1i(tex.uniformId, i);
		glBindTexture(GL_TEXTURE_2D, tex.id);
		i=i+1;
	}

	// Bind material
	glUniform1f(material.shininess_uniform, material.shininess);
	glUniform4f(material.diffuse_uniform,
			material.diffuse.x, material.diffuse.y, material.diffuse.z, material.diffuse.w);
	glUniform4f(material.ambient_uniform,
			material.ambient.x, material.ambient.y, material.ambient.z, material.ambient.w);
	glUniform1i(material.texCount_uniform, material.texCount);

	// Draw
	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES, this->indicesSize, GL_UNSIGNED_INT, 0);
}

void Mesh::setupMesh() {
	this->indicesSize=indices.size();

	// Create buffers/arrays
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glGenBuffers(1, &this->EBO);

	glBindVertexArray(this->VAO);
	
	// Load data into vertex buffers
	auto sizeof_vertex = sizeof(Vertex);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof_vertex, &this->vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

	// Set the vertex attribute pointers
	// Vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof_vertex, (GLvoid*)0);
	// Vertex Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof_vertex, (GLvoid*)offsetof(Vertex, Normal));
	// Vertex Texture Coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof_vertex, (GLvoid*)offsetof(Vertex, TexCoords));

	glBindVertexArray(0);
}


Mesh::~Mesh() {
	if(this->VAO!=0) 
		glDeleteVertexArrays(1, &this->VAO);

	if(this->VBO!=0)
		glDeleteBuffers(1, &this->VBO);

	if(this->EBO!=0)
		glDeleteBuffers(1, &this->EBO);

}
