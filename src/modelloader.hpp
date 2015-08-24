#ifndef MODELLOADER_H
#define MODELLOADER_H

#include <string>
#include <vector>

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
