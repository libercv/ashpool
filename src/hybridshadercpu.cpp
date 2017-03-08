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
#include "hybridshadercpu.hpp"
#include "camera.hpp"
#include "config.hpp"
#include "model.hpp"
#include "shaderprogram.hpp"
#include "window.hpp"
#include "world.hpp"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>
#include <string>
//#include <omp.h>


HybridShaderCPU::HybridShaderCPU(World *w)
    : gBufferShader{Config::gbuffer_shader_vert.c_str(),
                    Config::gbuffer_shader_frag.c_str()},
      world{w} {

  // omp_set_num_threads(6);
  // Initialize uniforms for the newly created Shaders
  world->initModelsUniforms(gBufferShader);

  // Initialize GBuffer and Scene textures
  init_pass1_gBuffer(); // GBuffer
  init_pass3_blit();    // Scene

  // We need GBuffer and Scene texture before initializing OpenCL
  init_pass2_lighting();
  
  init_geometry();
}

void HybridShaderCPU::init_geometry() {
  for(unsigned int i=0; i<world->bvh.totalNodes; i++) {
    BVH::LinearBVHNode clnode=world->bvh.nodes[i];
    
    bvhnodes.emplace_back(_BVHNode{glm::vec3(clnode.pMin.x, clnode.pMin.y, clnode.pMin.z),
                         glm::vec3(clnode.pMax.x, clnode.pMax.y, clnode.pMax.z),
                         clnode.primitivesOffset, clnode.nPrimitives, clnode.axis});        
  }
  
  for (Triangle cltriangle: world->bvh.primitives) {    
    triangles.emplace_back(_Triangle{glm::vec3(cltriangle.v1.x, cltriangle.v1.y, cltriangle.v1.z), 
                           glm::vec3(cltriangle.v2.x, cltriangle.v2.y, cltriangle.v2.z), 
                           glm::vec3(cltriangle.v3.x, cltriangle.v3.y, cltriangle.v3.z)});
  }
    
}



void HybridShaderCPU::render() {
  pass1_gBuffer();
  pass2_lighting();
  pass3_blit();
}

void HybridShaderCPU::init_pass1_gBuffer() {
  gBufferShader.use();
  glGenFramebuffers(1, &gBuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

  // - Position color buffer
  glGenTextures(1, &gPosition);
  glBindTexture(GL_TEXTURE_2D, gPosition);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, Config::window_width,
               Config::window_height, 0, GL_RGBA, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         gPosition, 0);

  // - Normal color buffer
  glGenTextures(1, &gNormal);
  glBindTexture(GL_TEXTURE_2D, gNormal);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Config::window_width,
               Config::window_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,
                         gNormal, 0);

  // - Color + Specular color buffer
  glGenTextures(1, &gAlbedoSpec);
  glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Config::window_width,
               Config::window_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D,
                         gAlbedoSpec, 0);

  // - Tell OpenGL which color attachments we'll use (of this framebuffer) for
  // rendering
  GLuint attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
                           GL_COLOR_ATTACHMENT2};
  glDrawBuffers(3, attachments);
  glGenRenderbuffers(1, &rboDepth);
  glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                        Config::window_width, Config::window_height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, rboDepth);

  // - Finally check if framebuffer is complete
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cout << "Framebuffer not complete!" << std::endl;
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Reserve host memory for GBuffer ans Scene
void HybridShaderCPU::init_pass2_lighting() {
  gAlbedoSpec_text.reserve(Config::window_height * Config::window_width *
                           sizeof(GLubyte) * 4);
  gNormal_text.reserve(Config::window_height * Config::window_width *
                       sizeof(GLubyte) * 4);
  gPosition_text.reserve(Config::window_height * Config::window_width *
                         sizeof(GLfloat) * 4);
  gScene_text.reserve(Config::window_height * Config::window_width *
                      sizeof(GLubyte) * 4);
}

// Initialize the framebuffer which will show the scene
void HybridShaderCPU::init_pass3_blit() {
  glGenFramebuffers(1, &gSceneBuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, gSceneBuffer);

  // - Scene will be rendered here
  glGenTextures(1, &gSceneTexture);
  glBindTexture(GL_TEXTURE_2D, gSceneTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Config::window_width,
               Config::window_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         gSceneTexture, 0);

  GLuint attachments[1] = {GL_COLOR_ATTACHMENT0};
  glDrawBuffers(1, attachments);

  // - Finally check if framebuffer is complete
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cout << "Framebuffer not complete!" << std::endl;
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Update the GBuffer
void HybridShaderCPU::pass1_gBuffer() {
  glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
  glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  gBufferShader.use();

  const Camera *camera = world->getCamera();
  const std::vector<Model> *models = world->getModels();

  auto vMat = camera->getViewMatrix();
  auto loc = gBufferShader.getUniformLocation(UNIFORM_VIEW_MATRIX);
  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(vMat));

  auto projMatrix = camera->getProjectionMatrix();
  auto projLoc = gBufferShader.getUniformLocation(UNIFORM_PROJECTION_MATRIX);
  glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));

  for (auto &m : *models) {
    glUniformMatrix4fv(gBufferShader.getUniformLocation(UNIFORM_MODEL_MATRIX),
                       1, GL_FALSE, glm::value_ptr(m.getModelMatrix()));
    m.draw(gBufferShader);
  }
}

glm::vec3 cltoglm(cl_float3 a) { return glm::vec3(a.x, a.y, a.z); }

// Generate final image from GBuffer + Geometry & Light information
void HybridShaderCPU::pass2_lighting() {
  // Bring GBuffer from GPU to HOST
  gBufferShader.use();
  glBindFramebuffer(GL_READ_FRAMEBUFFER, rboDepth);
  glReadBuffer(GL_COLOR_ATTACHMENT0);
  glReadPixels(0, 0, Config::window_width, Config::window_height, GL_RGBA,
               GL_FLOAT, gPosition_text.data());
  glReadBuffer(GL_COLOR_ATTACHMENT1);
  glReadPixels(0, 0, Config::window_width, Config::window_height, GL_RGBA,
               GL_UNSIGNED_BYTE, gNormal_text.data());
  glReadBuffer(GL_COLOR_ATTACHMENT2);
  glReadPixels(0, 0, Config::window_width, Config::window_height, GL_RGBA,
               GL_UNSIGNED_BYTE, gAlbedoSpec_text.data());

  // Loop through rows and columns
  glm::vec3 viewDir = world->getCamera()->getPosition();

  unsigned int yoffset = 0;

  for (unsigned int y = 0; y < Config::window_height; y++) {
#pragma omp parallel for schedule(dynamic)
    for (unsigned int x = 0; x < Config::window_width; x++) {
      unsigned int xoffset = yoffset + x * 4;
      // Get vectors to albedo, normal, color...
      glm::vec3 albedo =
          glm::vec3(gAlbedoSpec_text[xoffset], gAlbedoSpec_text[xoffset + 1],
                    gAlbedoSpec_text[xoffset + 2]);
      float specular = (float)(gAlbedoSpec_text[xoffset + 3])/255.0f;
      glm::vec3 normal =
          glm::normalize(glm::vec3(gNormal_text[xoffset], gNormal_text[xoffset + 1],
                    gNormal_text[xoffset + 2]));
      glm::vec3 position =
          glm::vec3(gPosition_text[xoffset], gPosition_text[xoffset + 1],
                    gPosition_text[xoffset + 2]);
      glm::vec3 ambient = albedo * world->scene_attribs.ambient;

      // Ambient light
      glm::vec3 out = ambient;

      // Point Lights
      out += pointLightsColor(position, normal, albedo, specular, viewDir);
      //out += pointLightsColor(position, normal, albedo, 0.0f, viewDir);

      // Output
      gScene_text[xoffset] = (GLubyte)(out.x); // * 255);
      gScene_text[xoffset + 1] = (GLubyte)(out.y); // * 255);
      gScene_text[xoffset + 2] = (GLubyte)(out.z); // * 255);
      gScene_text[xoffset + 3] = 0;
    }
    yoffset += Config::window_width * 4;
  }

#pragma omp critical
  // Update GPU Scene texture
  glBindTexture(GL_TEXTURE_2D, gSceneTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Config::window_width,
               Config::window_height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
               gScene_text.data());
}

glm::vec3 HybridShaderCPU::pointLightsColor(const glm::vec3 &position,
                                            const glm::vec3 &normal,
                                            const glm::vec3 &albedo,
                                            float specular,
                                            const glm::vec3 &viewDir) {
  glm::vec3 out{0.0f, 0.0f, 0.0f};

  // Compute each point light contribution
  for (PointLight l : world->getPointLights()) {
    
    
    glm::vec3 lpos = cltoglm(l.position);
    glm::vec3 lcol = cltoglm(l.color);
    glm::vec3 lightDir = glm::normalize(lpos - position);
    float dist = glm::distance(lpos, position);    
    if (world->scene_attribs.shadowsEnabled) {
       _Ray r = _Ray{position, lightDir, EPSILON, dist};
      if(intersects(&r))
        continue;
    }
    
    // Diffuse
    glm::vec3 diffuse =
        albedo * glm::max(glm::dot(normal, lightDir), 0.0f) * lcol;

    // Specular
    glm::vec3 halfwayDir = glm::normalize(lightDir + viewDir);
    float spec = glm::pow(glm::max(glm::dot(normal, halfwayDir), 0.0f), 16.0f);
    glm::vec3 l_spec = lcol * spec * specular;

    // Attenuation
    float distance = glm::length(lpos - position);
    float attenuation =
        1.0 / (1.0 + l.linear * distance + l.quadratic * distance * distance);

    out += (diffuse + l_spec) * attenuation;
  }
  return out;
}

// Copy the final image to the framebuffer
void HybridShaderCPU::pass3_blit() {
  glBindFramebuffer(GL_READ_FRAMEBUFFER, gSceneBuffer);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glBlitFramebuffer(0, 0, Config::window_width, Config::window_height, 0, 0,
                    Config::window_width, Config::window_height,
                    GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

HybridShaderCPU::~HybridShaderCPU() {}





// Checks ray-triangle intersection
// Based on algorithm in "Physically based rendering, 2nd edition"
bool HybridShaderCPU::test_ray_triangle(const _Triangle *tri, const _Ray *ray) {
  glm::vec3 e1 = tri->p1 - tri->p0;
  glm::vec3 e2 = tri->p2 - tri->p0;

  glm::vec3 s1 = glm::cross(ray->d, e2);
  float a = glm::dot(s1, e1);
  if (a > -EPSILON && a < EPSILON)
    return false;

  float invDivisor = 1.0f / a;

  glm::vec3 d = ray->o - tri->p0;
  float b1 = invDivisor * glm::dot(d, s1);
  if (b1 < 0.0f || b1 > 1.0f)
    return false;

  glm::vec3 s2 = glm::cross(d, e1);
  float b2 = invDivisor * glm::dot(ray->d, s2);
  if (b2 < 0.0f || b1 + b2 > 1.0f)
    return false;

  float t = invDivisor * glm::dot(e2, s2);
  if (t < ray->mint || t > ray->maxt)
    return false;
  else
    return true;
}

void swap(float *f1, float *f2) {
  float tmp = *f1;
  *f1 = *f2;
  *f2 = tmp;
}

// Checks ray-bounding box intersection
// Based on algorithm in "Physically based rendering, 2nd edition"
bool HybridShaderCPU::test_ray_bbox(const _Ray *ray, const _BVHNode *node,
                   const glm::vec3 *invDir) {
  float tmin = ray->mint;
  float tmax = ray->maxt;

  float tNear = (node->bounds_pMin.x - ray->o.x) * (*invDir).x;
  float tFar = (node->bounds_pMax.x - ray->o.x) * (*invDir).x;
  if (tNear > tFar)
    swap(&tNear, &tFar);
  tmin = tNear > tmin ? tNear : tmin;
  tmax = tFar < tmax ? tFar : tmax;
  if (tmin > tmax)
    return false;

  tNear = (node->bounds_pMin.y - ray->o.y) * (*invDir).y;
  tFar = (node->bounds_pMax.y - ray->o.y) * (*invDir).y;
  if (tNear > tFar)
    swap(&tNear, &tFar);
  tmin = tNear > tmin ? tNear : tmin;
  tmax = tFar < tmax ? tFar : tmax;
  if (tmin > tmax)
    return false;

  tNear = (node->bounds_pMin.z - ray->o.z) * (*invDir).z;
  tFar = (node->bounds_pMax.z - ray->o.z) * (*invDir).z;
  if (tNear > tFar)
    swap(&tNear, &tFar);
  tmin = tNear > tmin ? tNear : tmin;
  tmax = tFar < tmax ? tFar : tmax;
  if (tmin > tmax)
    return false;

  return true;
}

// Checks if a ray intersects with a collection of triangles, using
// a BVH tree to accelerate the process.
// Based on algorithm in "Physically based rendering, 2nd edition"
bool HybridShaderCPU::intersects(const _Ray *ray) {

  int todoOffset = 0, nodeNum = 0;
  int todo[64];

  glm::vec3 invDir =glm::vec3(1.0f / ray->d.x, 1.0f / ray->d.y, 1.0f / ray->d.z);
  int dirIsNeg[3] = {invDir.x < 0, invDir.y < 0, invDir.z < 0};

  while (true) {
    const _BVHNode *node = &bvhnodes[nodeNum];
    // Check ray against BVH node
    if (test_ray_bbox(ray, node, &invDir)) {
      if (node->nPrimitives > 0) {
        // Intersect ray with primitives in leaf BVH node
        for (int i = 0; i < node->nPrimitives; i++)
          if (test_ray_triangle(&triangles[node->uf.primitivesOffset + i], ray))
            return true;
        if (todoOffset == 0)
          break;
        nodeNum = todo[--todoOffset];
      } else {
        // Put far BVH node on todo stack, advance to near node
        if (dirIsNeg[node->axis]) {
          todo[todoOffset++] = nodeNum + 1;
          nodeNum = node->uf.secondChildOffset;
        } else {
          todo[todoOffset++] = node->uf.secondChildOffset;
          nodeNum = nodeNum + 1;
        }
      }
    } else {
      if (todoOffset == 0)
        break;
      nodeNum = todo[--todoOffset];
    }
  }
  return false;
}
