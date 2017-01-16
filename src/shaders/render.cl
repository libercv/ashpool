__constant sampler_t imageSampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;
__constant float EPSILON=0.01f;

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

bool test_ray_triangle(float3 p0, float3 p1, float3 p2,
                       float3 d, float3 o, float dis)  {
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



bool test_ray_intersection(__global const Meshes *meshes,
                           __global const float3 *vertices,
                           __global const int *indices,
                           int meshes_nr, float3 pos, float3 dir,
                           float dist) {
        
    float3 vertex1, vertex2, vertex3;   

    for (int mesh_nr=0; mesh_nr<meshes_nr; mesh_nr++) {            
		    Meshes m=meshes[mesh_nr];      
        int o = m.vertex_offset;
        
        for (int i=m.first_index; i<m.first_index+m.index_nr; i+=3) {              
           vertex1=vertices[indices[i]+o];
           vertex2=vertices[indices[i+1]+o];
           vertex3=vertices[indices[i+2]+o]; 
           
           if(test_ray_triangle(vertex1, vertex2, vertex3, dir, pos, dist))         
               return true;
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
                    __global const Meshes *meshes,
                    __global const float3 *vertices,
                    __global const int *indices,
                    int meshes_nr,
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
       
       bool shadows=test_ray_intersection(meshes, vertices, indices, 
                                      meshes_nr, gpos, light_dir, dist);       
       if (shadows==true)
          continue;         
     

       float ang=max(dot(gnormal,light_dir), 0.0f);       
       float3 diffuse=ang*gdiffuse*light_color;
       
       float attenuation=1.0f / (1.0f+(attribs.linear*dist)+(attribs.quadratic*dist*dist));
       diffuse *= attenuation;
       light += diffuse;              
    }
 
    float4 l=(float4)(light.x, light.y, light.z, 0.0f);
    write_imagef(output, coord, l);
}



