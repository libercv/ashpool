#ifndef MODELLOADER_H
#define MODELLOADER_H

#include <GL/glew.h>                 // for GLuint
#include <assimp/types.h>            // for aiString
#include <algorithm>                 // for move
#include <glm/glm.hpp>
#include <string>                    // for string
#include <vector>                    // for vector

struct aiScene;
struct aiMesh;
struct aiMaterial;
class Model;
class Mesh;
struct Material;

class ModelLoader {

public:
	ModelLoader() = default;
	static Model loadModel(const std::string& path);
private:
	static void loadMesh(std::vector<Mesh>& meshes, const aiMesh* mesh, const aiScene* scene, const std::string &directory);
	static Material loadMaterial(aiMaterial *mtl);

};
#endif // MODELLOADER_H
