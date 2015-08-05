// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>
#include <map>
#include <vector>
// GL Includes
#include <GL/glew.h> // Contains all the necessery OpenGL includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "model.h"

#include "mesh.h"


Model::Model(const std::string& path) {
	this->loadModel(path);
}

// Draws the model, and thus all its meshes
void Model::Draw(const Shader& shader) {
	for(GLuint i = 0; i < this->meshes.size(); i++)
		this->meshes[i].Draw(shader);
}
    
// Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
void Model::loadModel(const std::string& path) {
	// Read file via ASSIMP
	Assimp::Importer importer;
	std::cout << "Start importing model" << std::endl;
	const aiScene* scene = importer.ReadFile(path, aiProcess_GenNormals | aiProcess_Triangulate | aiProcess_FlipUVs); 
	std::cout << "Model imported." << std::endl;
	// Check for errors
	if(!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return;
	}
	// Retrieve the directory path of the filepath
	this->directory = path.substr(0, path.find_last_of('/'));

	std::cout << "Reservando " << scene->mNumMeshes << " meshes" << std::endl;
	// Process ASSIMP's root node recursively
	meshes.reserve(scene->mNumMeshes);
	std::cout << "Reservado" << std::endl;
	for(GLuint i = 0; i< scene->mNumMeshes; i++) {
		this->meshes.emplace_back(scene->mMeshes[i], scene, this->directory);
	}
}

