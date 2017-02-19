/***************************************************
 * HybridShaderCPU
 *
 * Renders the scene in 3 passes:
 * - Generate GBuffer with OpenGL
 * - Generate final Scene texture with OpenCL using
 *    GBuffer from previous pass and geometry (BVH) and lighting information
 * - Blit the result of the second pass to the render buffer (openGL)
 *
 * 2017 - Liberto Camús
 * **************************************************/
#ifndef HYBRIDSHADERCPU_H
#define HYBRIDSHADERCPU_H

#include "shaderprogram.hpp" // for ShaderProgram
#include <GL/glew.h>         // for GLuint
#include <glm/glm.hpp>
#include <memory>
#include <vector> // for vector
#include <vector>
class Camera;
class Model;
class World;

class HybridShaderCPU {
private:
  
  class _BVHNode {
  public:
    glm::vec3 bounds_pMin;
    glm::vec3 bounds_pMax;
    union {
      uint primitivesOffset;  // leaf
      uint secondChildOffset; // interior
    } uf;
    unsigned char nPrimitives; // 0 -> Interior
    unsigned char axis;        // interior node: xyz
    
    
  };
    
  class _Triangle {
  public :
    glm::vec3 p0, p1, p2; 
  };

  class _Ray  {
  public:
    glm::vec3 o; // origin
    glm::vec3 d; // direction
    float mint;
    float maxt;
  } ;
  
  
  static constexpr float EPSILON=0.001f;
  
  
  std::vector<_BVHNode> bvhnodes;
  std::vector<_Triangle> triangles;
  
  bool intersects(const _Ray *ray);
  bool test_ray_bbox(const _Ray *ray, const _BVHNode *node,
                     const glm::vec3 *invDir);
  bool test_ray_triangle(const _Triangle *tri, const _Ray *ray);
  
  // GBUFFER FRAMEBUFFER
  GLuint gBuffer;                         // Framebuffer
  GLuint gNormal, gPosition, gAlbedoSpec; // Color attachments
  GLuint rboDepth;

  // OPENCL RENDER FRAMEBUFFER
  GLuint gSceneBuffer;  // Framebuffer
  GLuint gSceneTexture; // Color attachment

  //
  std::vector<GLfloat> gNormal_text;
  std::vector<GLfloat> gPosition_text;
  std::vector<GLfloat> gAlbedoSpec_text;
  std::vector<GLubyte> gScene_text;

  ShaderProgram gBufferShader;
  World *world;

  void init_pass1_gBuffer();
  void init_pass2_lighting();
  void init_pass3_blit();
  void pass1_gBuffer();
  void pass2_lighting();
  void pass3_blit();

  const std::string GBUFFER_POSITION_TEXTURE = "gPosition";
  const std::string GBUFFER_ALBEDO_SPEC_TEXTURE = "gAlbedoSpec";
  const std::string GBUFFER_NORMAL_TEXTURE = "gNormal";
  const std::string GBUFFER_SCENE_TEXTURE = "gScene";

  const std::string UNIFORM_MODEL_MATRIX = "model";
  const std::string UNIFORM_VIEW_MATRIX = "view";
  const std::string UNIFORM_PROJECTION_MATRIX = "projection";

  glm::vec3 pointLightsColor(const glm::vec3 &position, const glm::vec3 &normal,
                             const glm::vec3 &albedo, float specular,
                             const glm::vec3 &viewDir);
  
  void init_geometry();
  

public:
  void render();
  const ShaderProgram &getModelShader() { return gBufferShader; }

  HybridShaderCPU(World *w);
  ~HybridShaderCPU();
};

#endif // HYBRIDSHADERCPU_H
