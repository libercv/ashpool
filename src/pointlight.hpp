/***************************************************
 * PointLight
 *
 * Contains properties of a Point Light in a format
 * suitable to be shared with OpenCL.
 * Stores position, color, attenuation values...
 *
 * 2017 - Liberto Camús
 * **************************************************/
#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include <CL/cl_gl.h>

class PointLight {
public:
  PointLight(float x, float y, float z)
      : position{{x, y, z}}, color{{1.0f, 1.0f, 1.0f}}, diffuse{0.8f},
        linear{0.001f}, quadratic{0.00001f}, shininess{1.0f} {}
  cl_float3 position;
  cl_float3 color;
  cl_float diffuse;
  cl_float linear;
  cl_float quadratic;
  cl_float shininess;
};

#endif // POINTLIGHT_H
