/***************************************************
 * Mesh
 *
 * Set of triangles, material properties, textures
 * and its methods for drawing and updating OpenGL
 * shaders.
 * A Model is made out of Meshes.
 *
 * 2017 - Liberto Camús
 * **************************************************/
#ifndef MESH_H
#define MESH_H

#include "material.hpp"
#include "texture.hpp"
#include "vertex.hpp"
#include <GL/glew.h> // for GLuint
#include <algorithm> // for move
#include <vector>    // for vector

#include <iostream>

class ShaderProgram;

class Mesh {
public:
  Mesh() = delete;
  Mesh(Mesh &&other)
      : vertices(std::move(other.vertices)), indices(std::move(other.indices)),
        specular_textures(std::move(other.specular_textures)),
        diffuse_textures(std::move(other.diffuse_textures)),
        normal_textures(std::move(other.normal_textures)),
        material(std::move(other.material)), VAO(other.VAO), VBO(other.VBO),
        EBO(other.EBO), indicesSize(other.indicesSize) {
    other.VBO = 0;
    other.EBO = 0;
    other.VAO = 0;
    other.indicesSize = 0;
  }

  Mesh(std::vector<Vertex> &&vertices, std::vector<GLuint> &&indices,
       std::vector<Texture> &&specular_textures,
       std::vector<Texture> &&diffuse_textures,
       std::vector<Texture> &&Normal_Textures, Material &&material);
  ~Mesh();

  Mesh &operator=(const Mesh &other) = delete;
  Mesh &operator=(const Mesh &&other) = delete;
  Mesh(const Mesh &other) = delete;

  void refreshUniforms(const ShaderProgram &shader);
  void draw(const ShaderProgram &shader) const;

  const std::vector<Vertex> &getVertices() const { return vertices; }
  const std::vector<GLuint> &getIndices() const { return indices; }

private:
  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;
  std::vector<Texture> specular_textures;
  std::vector<Texture> diffuse_textures;
  std::vector<Texture> normal_textures;
  struct Material material;
  GLuint VAO{0}, VBO{0}, EBO{0};
  GLuint indicesSize{0};

  void setupMesh();
};
#endif // MESH_H
