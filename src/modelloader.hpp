/***************************************************
 * ModelLoader
 *
 * Loads a model from a file into a Model object.
 * Uses ASSIMP to import different possible formats and it
 * loads meshes, textures and material properties
 *
 * 2017 - Liberto Camús
 * **************************************************/
#ifndef MODELLOADER_H
#define MODELLOADER_H

#include <GL/glew.h>
#include <string>
#include <vector>

#include "texturemanager.hpp"
#include "vertex.hpp"

struct aiScene;
struct aiMesh;
struct aiMaterial;
class Model;
class Mesh;
class Material;

class ModelLoader {

public:
  ModelLoader() = default;
  Model loadModel(const std::string &path);

private:
  std::vector<Mesh> loadMeshes(const aiScene *scene,
                               const std::string &directory);
  Material loadMaterial(const aiMaterial *mtl) const;
  std::vector<Vertex> loadMeshVertices(const aiMesh *mesh) const;
  std::vector<GLuint> loadMeshIndices(const aiMesh *mesh) const;
  TextureManager mTextureManager;
};
#endif // MODELLOADER_H
