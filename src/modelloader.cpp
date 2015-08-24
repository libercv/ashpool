#include "modelloader.hpp"
#include <GL/glew.h>                 // for GLuint, glBindVertexArray, etc
#include <assimp/postprocess.h>
#include <assimp/color4.h>           // for aiColor4D
#include <assimp/Importer.hpp>   // for Importer
#include <assimp/material.h>         // for aiGetMaterialColor, etc
#include <assimp/mesh.h>             // for aiMesh, aiFace
#include <assimp/scene.h>            // for aiScene
#include <assimp/vector3.h>          // for aiVector3t, aiVector3D
#include <ext/alloc_traits.h>        // for __alloc_traits<>::value_type
#include <stddef.h>                  // for offsetof
#include <assimp/material.inl>       // for aiMaterial::GetTextureCount
#include <iterator>                  // for back_insert_iterator, etc
#include <sstream>                   // for stringstream, basic_ostream
#include <iostream>
#include "mesh.hpp"
#include "model.hpp"
#include "shaderprogram.hpp"                // for Shader
#include "texturemanager.hpp"        // for TextureManager

Model ModelLoader::loadModel(const std::string& path) {
	Assimp::Importer importer;
	std::cout << "Start importing model" << std::endl;
	const aiScene* scene = importer.ReadFile(path, aiProcess_GenNormals |
			aiProcess_Triangulate | aiProcess_FlipUVs);

	// Check for errors
	if(!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE ||
			!scene->mRootNode) {
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		exit(2);
	}

	// Retrieve the directory path of the filepath
	auto directory = path.substr(0, path.find_last_of('/'));

	std::cout << "Reading " << scene->mNumMeshes << " meshes" << std::endl;
	std::vector<Mesh> meshes;
	meshes.reserve(scene->mNumMeshes);
	for(GLuint i = 0; i< scene->mNumMeshes; i++) {
		//meshes.emplace_back(scene->mMeshes[i], scene, directory);
		//meshes.push_back(std::move(this->loadMesh(scene->mMeshes[i], scene, directory)));
		ModelLoader::loadMesh(meshes, scene->mMeshes[i], scene, directory);
	}
	std::cout << "Model successfully imported." << std::endl;
	return Model(std::move(meshes));
}

void ModelLoader::loadMesh(std::vector<Mesh> &meshes, const aiMesh* mesh, const aiScene* scene, const std::string &directory) {
	std::vector<Vertex> vertices;
	vertices.reserve(mesh->mNumVertices);
	for(GLuint i = 0; i < mesh->mNumVertices; i++) {
		auto vv = &mesh->mVertices[i];
		auto vn = &mesh->mNormals[i];
		// Texture Coordinates
		if(mesh->mTextureCoords[0]) { // Does the mesh contain texture coordinates?
			auto vt = &mesh->mTextureCoords[0][i];
			// A vertex can contain up to 8 different texture coordinates. We thus make the assumption that
			// we won't use models where a vertex can have multiple texture coordinates so we always take
			// the first set (0).
			vertices.emplace_back( glm::vec3(vv->x, vv->y, vv->z),
					glm::vec3(vn->x, vn->y, vn->z),
					glm::vec2(vt->x, vt->y)  );
		} else {
			vertices.emplace_back( glm::vec3(vv->x, vv->y, vv->z),
					glm::vec3(vn->x, vn->y, vn->z),
					glm::vec2(0.0f, 0.0f)  );
		}
	}
	// Now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding
	// vertex indices.
	std::vector<GLuint> indices;
	indices.reserve(3 * mesh->mNumFaces);
	for(GLuint i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		std::copy_n(&face.mIndices[0], face.mNumIndices, back_inserter(indices) );
		//indices.insert(indices.end(), &face.mIndices[0], &face.mIndices[face.mNumIndices]);
	}

	// Process materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	// We assume a convention for sampler names in the shaders. Each diffuse texture should be named
	// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
	// Same applies to other texture as the following list summarizes:
	// Diffuse: texture_diffuseN
	// Specular: texture_specularN
	// Normal: texture_normalN

	std::vector<Texture> textures;

	// 1. Diffuse maps
	std::vector<Texture> diffuseMaps = TextureManager::get().loadMaterialTextures(material,
			aiTextureType_DIFFUSE, "texture_diffuse", directory);
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	// 2. Specular maps
	std::vector<Texture> specularMaps = TextureManager::get().loadMaterialTextures(material,
			aiTextureType_SPECULAR, "texture_specular", directory);
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

	auto mat = ModelLoader::loadMaterial(material);
	//glGenBuffers(1,&uniformBlockIndex);
	//glBindBuffer(GL_UNIFORM_BUFFER, uniformBlockIndex);
	//glBufferData(GL_UNIFORM_BUFFER, sizeof(mat), (void *)(&mat), GL_STATIC_DRAW);
	meshes.emplace_back(std::move(vertices), std::move(indices), std::move(textures), std::move(mat));
}

Material ModelLoader::loadMaterial(aiMaterial *mtl){
	Material mat;

	if(mtl->GetTextureCount(aiTextureType_DIFFUSE)>0)
		mat.texCount = 1;

	aiColor4D color;
	if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &color))
		mat.diffuse = glm::vec4(color.r, color.g, color.b, color.a);

	if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &color))
		mat.ambient = glm::vec4(color.r, color.g, color.b, color.a);

	if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &color))
		mat.specular=glm::vec4(color.r, color.g, color.b, color.a);

	if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_EMISSIVE, &color))
		mat.emissive=glm::vec4(color.r, color.g, color.b, color.a);

	unsigned int max;
	aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS, &mat.shininess, &max);

	return mat;
}


