/***************************************************
 * ModelLoader
 *
 * Loads a model from a file into a Model object.
 * Uses ASSIMP to import different possible formats and it
 * loads meshes, textures and material properties
 *
 * 2017 - Liberto Camús
 * **************************************************/
#include "modelloader.hpp"
#include "material.hpp"
#include "mesh.hpp"
#include "model.hpp"
#include "shaderprogram.hpp" // for Shader
#include "texture.hpp"
#include "texturemanager.hpp" // for TextureManager
#include "vertex.hpp"
#include <GL/glew.h>           // for GLuint, glBindVertexArray, etc
#include <assimp/Importer.hpp> // for Importer
#include <assimp/color4.h>     // for aiColor4D
#include <assimp/material.h>   // for aiGetMaterialColor, etc
#include <assimp/material.inl> // for aiMaterial::GetTextureCount
#include <assimp/mesh.h>       // for aiMesh, aiFace
#include <assimp/postprocess.h>
#include <assimp/scene.h>     // for aiScene
#include <assimp/vector3.h>   // for aiVector3t, aiVector3D
#include <ext/alloc_traits.h> // for __alloc_traits<>::value_type
#include <iostream>
#include <iterator> // for back_insert_iterator, etc
#include <sstream>  // for stringstream, basic_ostream
#include <stddef.h> // for offsetof

Model ModelLoader::loadModel(const std::string &path) {
  // Read file
  std::cout << "Start importing model" << std::endl;
  Assimp::Importer importer;
  auto directory = path.substr(0, path.find_last_of('/'));
  const aiScene *scene = importer.ReadFile(
      path, aiProcess_GenSmoothNormals | aiProcess_Triangulate);

  // Check for errors
  if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    std::cout << "Error importing model " << importer.GetErrorString()
              << std::endl;
    exit(2);
  }

  // Return new model
  return Model(loadMeshes(scene, directory));
}

std::vector<Mesh> ModelLoader::loadMeshes(const aiScene *scene,
                                          const std::string &directory) {

  std::vector<Mesh> meshes;
  std::cout << "Reading " << scene->mNumMeshes << " meshes" << std::endl;
  meshes.reserve(scene->mNumMeshes);
  for (GLuint i = 0; i < scene->mNumMeshes; i++) {
    auto m = scene->mMeshes[i];
    aiMaterial *mat = scene->mMaterials[m->mMaterialIndex];

    meshes.emplace_back(loadMeshVertices(m), loadMeshIndices(m),
                        mTextureManager.loadMaterialTextures(
                            mat, aiTextureType_SPECULAR, directory),
                        mTextureManager.loadMaterialTextures(
                            mat, aiTextureType_DIFFUSE, directory),
                        std::vector<Texture>(), loadMaterial(mat));
  }

  std::cout << "Model successfully imported." << std::endl;
  return meshes;
}

std::vector<Vertex> ModelLoader::loadMeshVertices(const aiMesh *mesh) const {

  std::vector<Vertex> vertices;
  vertices.reserve(mesh->mNumVertices);

  for (GLuint i = 0; i < mesh->mNumVertices; i++) {
    auto vv = &mesh->mVertices[i];
    auto vn = &mesh->mNormals[i];
    // Texture Coordinates
    if (mesh->mTextureCoords[0]) {
      // Load only 1st texture coordinates set. It could have up to 8.
      auto vt = &mesh->mTextureCoords[0][i];

      vertices.emplace_back(glm::vec3(vv->x, vv->y, vv->z),
                            glm::vec3(vn->x, vn->y, vn->z),
                            glm::vec2(vt->x, vt->y));
    } else {
      vertices.emplace_back(glm::vec3(vv->x, vv->y, vv->z),
                            glm::vec3(vn->x, vn->y, vn->z),
                            glm::vec2(0.0f, 0.0f));
    }
  }
  return vertices;
}

std::vector<GLuint> ModelLoader::loadMeshIndices(const aiMesh *mesh) const {

  std::vector<GLuint> indices;
  indices.reserve(3 * mesh->mNumFaces);

  for (GLuint i = 0; i < mesh->mNumFaces; i++) {
    aiFace *face = &mesh->mFaces[i];
    std::copy_n(&face->mIndices[0], face->mNumIndices, back_inserter(indices));
  }

  return indices;
}

Material ModelLoader::loadMaterial(const aiMaterial *mtl) const {

  Material mat;

  mat.texCount = 0;

  if (mtl->GetTextureCount(aiTextureType_DIFFUSE) > 0)
    mat.texCount = 1;

  aiColor4D color;
  if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &color))
    mat.diffuse = glm::vec4(color.r, color.g, color.b, color.a);

  if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &color))
    mat.ambient = glm::vec4(color.r, color.g, color.b, color.a);

  if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &color))
    mat.specular = glm::vec4(color.r, color.g, color.b, color.a);

  if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_EMISSIVE, &color))
    mat.emissive = glm::vec4(color.r, color.g, color.b, color.a);

  return mat;
}
