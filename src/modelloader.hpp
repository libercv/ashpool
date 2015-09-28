#ifndef MODELLOADER_H
#define MODELLOADER_H

#include <string>
#include <vector>
#include <GL/glew.h>

#include "vertex.hpp"
#include "texturemanager.hpp"

struct aiScene;
struct aiMesh;
struct aiMaterial;
class Model;
class Mesh;
struct Material;

class ModelLoader {

public:
	ModelLoader() = default;
	Model loadModel(const std::string& path) ;
private:
	std::vector<Mesh> loadMeshes(const aiScene* scene, const std::string &directory) ;
	Material loadMaterial(const aiMaterial* mtl) ;
	std::vector<Vertex> loadMeshVertices(const aiMesh* mesh) ;
	std::vector<GLuint> loadMeshIndices(const aiMesh* mesh) ; 
	TextureManager mTextureManager;
};
#endif // MODELLOADER_H
