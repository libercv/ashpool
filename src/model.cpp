#include "model.hpp"
#include <GL/glew.h>             // for GLuint
#include <assimp/postprocess.h>
#include <assimp/scene.h>        // for aiScene, AI_SCENE_FLAGS_INCOMPLETE
#include <assimp/Importer.hpp>   // for Importer
#include <iostream>              // for operator<<, basic_ostream, cout, etc
#include <string>                // for string, allocator, char_traits
#include <vector>                // for vector
#include "mesh.hpp"              // for Mesh
struct aiMesh;


Model::Model(const std::string& path) {
	Assimp::Importer importer;
	std::cout << "Start importing model" << std::endl;
	const aiScene* scene = importer.ReadFile(path, aiProcess_GenNormals |
		aiProcess_Triangulate | aiProcess_FlipUVs);

	// Check for errors
	if(!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE ||
		!scene->mRootNode) {
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return;
	}

	// Retrieve the directory path of the filepath
	this->directory = path.substr(0, path.find_last_of('/'));

	std::cout << "Reading " << scene->mNumMeshes << " meshes" << std::endl;
	meshes.reserve(scene->mNumMeshes);
	for(GLuint i = 0; i< scene->mNumMeshes; i++) {
		this->meshes.emplace_back(scene->mMeshes[i], scene, this->directory);
	}
	std::cout << "Model successfully imported." << std::endl;
}

// Draws the model, and thus all its meshes
void Model::Draw(const Shader& shader) const {
	for (const auto& mesh: meshes)
		mesh.Draw(shader);
}
