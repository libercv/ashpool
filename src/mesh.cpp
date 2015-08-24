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



Mesh::Mesh(std::vector<Vertex> Vertices,  std::vector<GLuint> Indices, std::vector<Texture> Textures, Material material) :
	vertices {std::move(Vertices)}, 
	indices {std::move(Indices)}, 
	textures {std::move(Textures)},
	mat {std::move(material)} {
		this->setupMesh();
	}



// Render the mesh
void  Mesh::Draw(const ShaderProgram& shader) const {
	shader.use();
	// Bind appropriate textures
	GLuint diffuseNr = 1;
	GLuint specularNr = 1;
	for(GLuint i = 0; i < this->textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i); // Active proper texture unit before binding
		// Retrieve texture number (the N in diffuse_textureN)
		std::stringstream ss;
		std::string number;
		std::string name = this->textures[i].type;
		if(name == "texture_diffuse")
			ss << diffuseNr++; // Transfer GLuint to stream
		else if(name == "texture_specular")
			ss << specularNr++; // Transfer GLuint to stream
		number = ss.str();
		// Now set the sampler to the correct texture unit
		glUniform1i(shader.getUniformLocation(name + number), i);
		// And finally bind the texture
		glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
	}

	// Also set each mesh's shininess property to a default value (if you want you could extend this to another
	// mesh property and possibly change this value)
	glUniform1f(shader.getUniformLocation("material_shininess"), mat.shininess);
	glUniform4f(shader.getUniformLocation("material_diffuse"),
			mat.diffuse.x, mat.diffuse.y, mat.diffuse.z, mat.diffuse.w);
	glUniform4f(shader.getUniformLocation("material_ambient"),
			mat.ambient.x, mat.ambient.y, mat.ambient.z, mat.ambient.w);
	glUniform1i(shader.getUniformLocation("material_texCount"), mat.texCount);
	//glBindBufferRange(GL_UNIFORM_BUFFER, materialUniLoc, myMeshes[nd->mMeshes[n]].uniformBlockIndex,
	// 0, sizeof(struct MyMaterial));

	// Draw mesh
	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// Always good practice to set everything back to defaults once configured.
	//for (GLuint i = 0; i < this->textures.size(); i++) {
	//	glActiveTexture(GL_TEXTURE0 + i);
	//	glBindTexture(GL_TEXTURE_2D, 0);
	//}
}

void Mesh::setupMesh() {
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
