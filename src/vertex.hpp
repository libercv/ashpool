#ifndef VERTEX_H
#define VERTEX_H
#include <GL/glew.h>      // for GLuint
#include <algorithm>      // for move
#include <assimp/types.h> // for aiString
#include <glm/glm.hpp>
#include <string> // for string
#include <vector> // for vector

class Vertex {
public:
  Vertex() = default;
  Vertex(Vertex &&other)
      : Position(std::move(other.Position)), Normal(std::move(other.Normal)),
        TexCoords(std::move(other.TexCoords)), MTangent(std::move(other.MTangent)),
        BiTangent(std::move(other.BiTangent)){}
  Vertex(glm::vec3 pos, glm::vec3 nor, glm::vec2 tex, glm::vec3 mtan, glm::vec3 bitan)
      : Position(std::move(pos)), Normal(std::move(nor)),
        TexCoords(std::move(tex)), MTangent(std::move(mtan)), BiTangent(std::move(bitan)) {}
  Vertex &operator=(const Vertex &other) = default;
  Vertex(const Vertex &other) = default;

  glm::vec3 Position;
  glm::vec3 Normal;
  glm::vec2 TexCoords;
  glm::vec3 MTangent;
  glm::vec3 BiTangent;
};

#endif // VERTEX_H
