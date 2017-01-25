__constant sampler_t imageSampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;
__constant float EPSILON=0.0001f;


typedef struct _BVHNode {
  float3 bounds_pMin;
  float3 bounds_pMax;
  union {
    uint primitivesOffset; // leaf
    uint secondChildOffset; // interior    
  } uf;
  uchar nPrimitives; // 0 -> Interior
  uchar axis; // interior node: xyz
  uchar pad[2]; // ensure 32 byte total size
} BVHNode;


typedef struct _Ray {
  float3 o; // origin
  float3 d; // direction
  float mint;
  float maxt;
} Ray;

typedef struct _Triangle {
  float3 p0, p1, p2;  
} Triangle;

typedef struct _PointLight {
  float3 p_position;
  float3 p_color;
} PointLight;

typedef struct _SceneAttribs {
  float diffuse;
  float linear;
  float quadratic;
} SceneAttribs;

typedef struct _Meshes {
   int vertex_offset;
   int first_index;
   int index_nr;
} Meshes;

bool test_ray_triangle_points(const float3 p0, const float3 p1, const float3 p2,
                       const float3 d, const float3 o, const float dis)  {
   float3 e1=p1-p0;
   float3 e2=p2-p0;
   float3 q=cross(d,e2);
   float a=dot(e1, q);   
   if(a>-EPSILON && a<EPSILON) return false;
   
   float f = 1.0f/a;
   float3 s=o-p0;
   float u=f*dot(s,q);
   if(u<0.0f) return false;
   
   float3 r=cross(s,e1);
   float v=f*dot(d,r);
   if(v<0.0f || u+v>1.0f) return false;

   float t=f*dot(e2,r);
   if (t>EPSILON && t<dis)
     return true;
   else         
     return false;     
}



bool test_ray_triangle(__global const Triangle *tri, const Ray *ray)  {
  float3 e1 = tri->p1 - tri->p0;
  float3 e2 = tri->p2 - tri->p0;
  
  float3 s1 = cross(ray->d, e2);
  float a=dot(s1, e1);   
  if(a>-EPSILON && a<EPSILON) return false;
   
  float invDivisor = 1.0f/a;
   
  float3 d=ray->o - tri->p0;
  float b1=invDivisor*dot(d,s1);
  if(b1<0.0f || b1>1.0f) return false;
   
  float3 s2=cross(d,e1);
  float b2=invDivisor*dot(ray->d,s2);
  if(b2<0.0f || b1+b2>1.0f) return false;

  float t=invDivisor*dot(e2,s2);
  if (t<ray->mint || t>ray->maxt)   
     return false;
   else         
     return true;     
}

/*

bool test_ray_intersection(__global const Meshes *meshes,
                           __global const float3 *vertices,
                           __global const int *indices,
                           int meshes_nr, float3 pos, float3 dir,
                           float dist) {
        
    float3 vertex1, vertex2, vertex3;   

    for (int mesh_nr=0; mesh_nr<meshes_nr; mesh_nr++) {            
		    Meshes m=meshes[mesh_nr];      
        int o = m.vertex_offset;   
        
        if(m.index_nr>300)
           continue;


        for (int i=m.first_index; i<m.first_index+m.index_nr; i+=3) {              
           
           vertex1=vertices[indices[i]+o];
           //float dist_vertex=distance(pos,vertex1);
           //if (dist_vertex>dist || dist_vertex>5)
           //    continue;           

           vertex2=vertices[indices[i+1]+o]; 
           vertex3=vertices[indices[i+2]+o]; 
           
           //if(test_ray_triangle(vertex1, vertex2, vertex3, dir, pos, dist))         
               return true;
        }
    }
    return false;
  
}
*/

void swap(float *f1, float *f2) {
  float tmp;
  tmp = *f1;
  *f1 = *f2;
  *f2 = tmp;
}

bool test_ray_bbox(const Ray *ray, __global const BVHNode *node) {
 
  //Update interval for ith bounding box slab   
  float tmin = ray->mint;
  float tmax = ray->maxt;  

  float invRayDir = 1.f / ray->d.x;
  float tNear = (node->bounds_pMin.x - ray->o.x) * invRayDir;
  float tFar = (node->bounds_pMax.x - ray->o.x) * invRayDir;
  
  if (tNear > tFar) { swap( &tNear, &tFar); }   
  tmin = tNear > tmin ? tNear : tmin;
  tmax = tFar < tmax ? tFar : tmax;
  if (tmin > tmax) return false;    
  
  
  invRayDir = 1.f / ray->d.y;
  tNear = (node->bounds_pMin.y - ray->o.y) * invRayDir;
  tFar = (node->bounds_pMax.y - ray->o.y) * invRayDir;
  if (tNear > tFar) { swap( &tNear, &tFar); }   

  tmin = tNear > tmin ? tNear : tmin;
  tmax = tFar < tmax ? tFar : tmax;
  if (tmin > tmax) return false;    

  
  invRayDir = 1.f / ray->d.z;
  tNear = (node->bounds_pMin.z - ray->o.z) * invRayDir;
  tFar = (node->bounds_pMax.z - ray->o.z) * invRayDir;

  if (tNear > tFar) { swap( &tNear, &tFar); }   
  tmin = tNear > tmin ? tNear : tmin;
  tmax = tFar < tmax ? tFar : tmax;
  if (tmin > tmax) return false;    
  
  return true;
}

/*
__kernel void render(__read_only image2d_t g_albedo_spec,
                    __read_only image2d_t g_position,
                    __read_only image2d_t g_normal,
                    __global const float *point_lights,
                    int point_lights_nr,
                    const SceneAttribs attribs,
                    __global const Meshes *meshes,
                    __global const float3 *vertices,
                    __global const int *indices,
                    int meshes_nr,
                    __write_only image2d_t output)
                    */
                    
bool intersects(const Ray *ray,  __global const Triangle *triangles, 
  __global const BVHNode *nodes) {
  
  bool hit=false;
  int todoOffset = 0, nodeNum = 0;
  int todo[64];
  
  float3 invDir=(float3)(1.0f / ray->d.x, 1.0f / ray->d.y, 1.0f / ray->d.z);
  int dirIsNeg[3] = { invDir.x < 0, invDir.y < 0, invDir.z < 0 };
  
  while (true) {
   const __global BVHNode *node = &nodes[nodeNum];
   //Check ray against BVH node
   if (test_ray_bbox(ray, node)) {
     if (node->nPrimitives > 0) {
       // Intersect ray with primitives in leaf BVH node   
       for (int i = 0; i < node->nPrimitives; i++)
         if (test_ray_triangle(&triangles[node->uf.primitivesOffset+i], ray)) 
           return true;        
       if (todoOffset == 0) break;
       nodeNum = todo[--todoOffset];
     } else {
       //Put far BVH node on todo stack, advance to near node   
       if (dirIsNeg[node->axis]) {
         todo[todoOffset++] = nodeNum + 1;
         nodeNum = node->uf.secondChildOffset;
       }
       else {
         todo[todoOffset++] = node->uf.secondChildOffset;
         nodeNum = nodeNum + 1;
       }
     }
   } else {
     if (todoOffset == 0) break;
     nodeNum = todo[--todoOffset];
   }
  }
  return false;
}





__kernel void render(__read_only image2d_t g_albedo_spec,
                    __read_only image2d_t g_position,
                    __read_only image2d_t g_normal,
                    __global const float *point_lights,
                    int point_lights_nr,
                    const SceneAttribs attribs,
                    __global const Triangle *triangles,
                    __global const BVHNode *nodes,                    
                    __write_only image2d_t output)
{

    int2 coord = (int2)(get_global_id(0), get_global_id(1));
    float3 gdiffuse = read_imagef(g_albedo_spec, imageSampler, coord).xyz;
    float3 gpos=read_imagef(g_position, imageSampler, coord).xyz;
    float3 gnormal=read_imagef(g_normal, imageSampler, coord).xyz;
    
    float3 light_color=(float3)(1.0f,1.0f,1.0f);
    float3 light=gdiffuse*attribs.diffuse;    
    for(int i=0; i<point_lights_nr; i++)    
    {
       float3 light_pos=(float3)(point_lights[i*3], point_lights[i*3+1], point_lights[i*3+2]);
       float3 light_dir=normalize(light_pos-gpos);
       float dist=distance(light_pos,gpos);
       
       Ray r;
       r.o = gpos;
       r.d = light_dir;
       r.mint = EPSILON;
       r.maxt = dist;
       bool shadows=intersects( &r, triangles, nodes);
       if (shadows==true)
         continue;
       
       //bool shadows=test_ray_intersection(meshes, vertices, indices, 
       //                               meshes_nr, gpos, light_dir, dist);       
       //if (shadows==true)
       //   continue;         
     

       float ang=max(dot(gnormal,light_dir), 0.0f);       
       float3 diffuse=ang*gdiffuse*light_color;
       
       float attenuation=1.0f / (1.0f+(attribs.linear*dist)+(attribs.quadratic*dist*dist));
       diffuse *= attenuation;
       light += diffuse;              
    }
 
    float4 l=(float4)(light.x, light.y, light.z, 0.0f);
    write_imagef(output, coord, l);
}



