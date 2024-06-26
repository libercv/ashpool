/*********************************************************************
*  render.cl
*
*  Uses G-Buffer textures (Diffuse+Specular, Positions, Normals),
*  a Bounding Volume Hierarchy containing the geometry (triangles) and
*  lighting information to produce a rendered texture.
*
*  2017 - Liberto Camús
************************************************************************/
__constant sampler_t imageSampler =
    CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;

// Minimum distance to avoid self-intersection
__constant float EPSILON = 0.001f;
// Threshold to discard light contribution
__constant float ATTENUATION_SENSITIVITY = 0.02f;

// Struct of each Bounding Volume Hierarchy node
typedef struct _BVHNode {
  float3 bounds_pMin;
  float3 bounds_pMax;
  union {
    uint primitivesOffset;  // leaf
    uint secondChildOffset; // interior
  } uf;
  uchar nPrimitives; // 0 -> Interior
  uchar axis;        // interior node: xyz
} BVHNode;

typedef struct _Ray {
  float3 o; // origin
  float3 d; // direction
  float mint;
  float maxt;
} Ray;

typedef struct _Triangle { float3 p0, p1, p2; } Triangle;

typedef struct _PointLight {
  float3 p_position;
  float3 p_color;
  float intensity;
  float linear;
  float quadratic;
} PointLight;

typedef struct _SceneAttribs {
  float ambient;
  int shadows_enabled;
} SceneAttribs;

// Checks ray-triangle intersection
// Based on algorithm in "Physically based rendering, 2nd edition"
bool test_ray_triangle(__global const Triangle *tri, const Ray *ray) {
  float3 e1 = tri->p1 - tri->p0;
  float3 e2 = tri->p2 - tri->p0;

  float3 s1 = cross(ray->d, e2);
  float a = dot(s1, e1);
  if (a > -EPSILON && a < EPSILON)
    return false;

  float invDivisor = 1.0f / a;

  float3 d = ray->o - tri->p0;
  float b1 = invDivisor * dot(d, s1);
  if (b1 < 0.0f || b1 > 1.0f)
    return false;

  float3 s2 = cross(d, e1);
  float b2 = invDivisor * dot(ray->d, s2);
  if (b2 < 0.0f || b1 + b2 > 1.0f)
    return false;

  float t = invDivisor * dot(e2, s2);
  if (t < ray->mint || t > ray->maxt)
    return false;
  else
    return true;
}

// Checks ray-bounding box intersection
// Based on algorithm in "Physically based rendering, 2nd edition"
bool test_ray_bbox(const Ray *ray, __global const BVHNode *node,
                   const float3 *invDir) {

  float3 tNear = (node->bounds_pMin - ray->o) * (*invDir);
  float3 tFar = (node->bounds_pMax - ray->o) * (*invDir);

  float tmin = max(ray->mint, min(tNear.x, tFar.x));
  float tmax = min(ray->maxt, max(tNear.x, tFar.x));
  if (tmin > tmax)
    return false;

  tmin = max(tmin, min(tNear.y, tFar.y));
  tmax = min(tmax, max(tNear.y, tFar.y));
  if (tmin > tmax)
    return false;

  tmin = max(tmin, min(tNear.z, tFar.z));
  tmax = min(tmax, max(tNear.z, tFar.z));
  if (tmin > tmax)
    return false;

  return true;
}

// Checks if a ray intersects with a collection of triangles, using
// a BVH tree to accelerate the process.
// Based on algorithm in "Physically based rendering, 2nd edition"
bool intersects(const Ray *ray, __global const Triangle *triangles,
                __global const BVHNode *nodes) {

  int todoOffset = 0, nodeNum = 0;
  int todo[64];

  float3 invDir = (float3)(1.0f / ray->d.x, 1.0f / ray->d.y, 1.0f / ray->d.z);
  // int dirIsNeg[3] = {invDir.x < 0, invDir.y < 0, invDir.z < 0};

  while (true) {
    const __global BVHNode *node = &nodes[nodeNum];
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
        // if (dirIsNeg[node->axis]) {
        todo[todoOffset++] = nodeNum + 1;
        nodeNum = node->uf.secondChildOffset;
        //} else {
        //  todo[todoOffset++] = node->uf.secondChildOffset;
        //  nodeNum = nodeNum + 1;
        //}
      }
    } else {
      if (todoOffset == 0)
        break;
      nodeNum = todo[--todoOffset];
    }
  }
  return false;
}

// Calculates point lights contribution to shading a specific point
// on a surface
float3 pointLightsColor(__constant PointLight *point_lights,
                        const int point_lights_nr, const float3 view_pos,
                        const float3 surface_pos, const float3 surface_normal,
                        const float3 surface_diffuse,
                        const float surface_specular, bool shadows_enabled,
                        __global const Triangle *triangles,
                        __global const BVHNode *nodes) {

  float3 color = (float3)(0.0f, 0.0f, 0.0f);
  const float3 view_dir = fast_normalize(view_pos - surface_pos);

  for (int i = 0; i < point_lights_nr; i++) {
    __constant PointLight *pLight = &point_lights[i];

    // Attenuation
    const float dist = fast_distance(pLight->p_position, surface_pos);
    const float attenuation = 1.0f / (1.0f + (pLight->linear * dist) +
                                      (pLight->quadratic * dist * dist));
    if (attenuation < ATTENUATION_SENSITIVITY && shadows_enabled)
      continue;

    const float3 light_dir = fast_normalize(pLight->p_position - surface_pos);
    if (shadows_enabled) {
      Ray r = (Ray){surface_pos, light_dir, EPSILON, dist};
      if (intersects(&r, triangles, nodes))
        continue;
    }

    // Diffuse Lambertian
    const float ang = max(dot(surface_normal, light_dir), 0.0f);
    const float3 diffuse =
        ang * surface_diffuse * pLight->p_color * pLight->intensity;
    color += diffuse * attenuation;

    // Specular Blinn-Phong
    if (surface_specular > 0.0f) {
      const float3 halfway_dir = fast_normalize(light_dir + view_dir);
      const float spec =
          pow(max(dot(surface_normal, halfway_dir), 0.0f), 16.0f);
      const float3 specular = pLight->p_color * spec * surface_specular;
      color += specular * attenuation;
    }
  }

  return color;
}

//
// Main function. Kernel "render"
//
__kernel void
render(__read_only image2d_t g_albedo_spec, __read_only image2d_t g_position,
       __read_only image2d_t g_normal, __write_only image2d_t output,
       const SceneAttribs attribs, const float3 view_position,
       __constant PointLight *point_lights, const int point_lights_nr,
       __global const Triangle *triangles, __global const BVHNode *nodes) {

  int2 coord = (int2)(get_global_id(0), get_global_id(1));
  const float3 diffuse = read_imagef(g_albedo_spec, imageSampler, coord).xyz;
  const float specular = read_imagef(g_albedo_spec, imageSampler, coord).w;
  const float3 pos = read_imagef(g_position, imageSampler, coord).xyz;
  const float3 norm = read_imagef(g_normal, imageSampler, coord).xyz;
  const float3 normal = fast_normalize(2 * norm - (float3)(1, 1, 1));

  // Ambient light
  float3 color = diffuse * attribs.ambient;

  // Point Lights (diffuse + specular)
  color += pointLightsColor(point_lights, point_lights_nr, view_position, pos,
                            normal, diffuse, specular,
                            (bool)attribs.shadows_enabled, triangles, nodes);

  // Write result
  write_imagef(output, coord, (float4)(color.x, color.y, color.z, 0.0f));
}
