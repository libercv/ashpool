#ifndef MODELLOADER_H
#define MODELLOADER_H

#include <string>
#include <vector>
#include <GL/glew.h>

#include "vertex.hpp"

struct aiScene;
struct aiMesh;
struct aiMaterial;
class Model;
class Mesh;
class Dumb;
struct Material;

class ModelLoader {

public:
	ModelLoader() = default;
	static Model loadModel(const std::string& path);
private:
	static std::vector<Mesh> loadMeshes(const aiScene* scene, const std::string &directory);
	static Material loadMaterial(const aiMaterial* mtl);
	static std::vector<Vertex> loadMeshVertices(const aiMesh* mesh);
	static std::vector<GLuint> loadMeshIndices(const aiMesh* mesh); 
	static Dumb loadMeshDumb();

};
#endif // MODELLOADER_H
