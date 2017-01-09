__constant sampler_t imageSampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;


typedef struct _PointLight {
  float3 p_position;
  float3 p_color;
} PointLight;

typedef struct _SceneAttribs {
  float diffuse;
  float linear;
  float quadratic;
} SceneAttribs;

__kernel void render(__read_only image2d_t g_albedo_spec,
                    __read_only image2d_t g_position,
                    __read_only image2d_t g_normal,
                    __global const float *point_lights,
                    int nr_point_lights,
                    SceneAttribs attribs,
                    __write_only image2d_t output)
{
    int2 coord = (int2)(get_global_id(0), get_global_id(1));
    float3 gdiffuse = read_imagef(g_albedo_spec, imageSampler, coord).xyz;
    float3 gpos=read_imagef(g_position, imageSampler, coord).xyz;
    float3 gnormal=read_imagef(g_normal, imageSampler, coord).xyz;
    
    float3 light_color=(float3)(1.0f,1.0f,1.0f);
    float3 light=gdiffuse*attribs.diffuse;
    for(int i=0; i<nr_point_lights; i++)
    {
       float3 light_pos=(float3)(point_lights[i*3], point_lights[i*3+1], point_lights[i*3+2]);
       float3 light_dir=normalize(light_pos-gpos);
       float ang=max(dot(gnormal,light_dir), 0.0f);       
       float3 diffuse=ang*gdiffuse*light_color;
       float dist=distance(light_pos,gpos);
       float attenuation=1.0f / (1.0f+(attribs.linear*dist)+(attribs.quadratic*dist*dist));
       diffuse *= attenuation;
       light += diffuse;       
    }

    float4 l=(float4)(light.x, light.y, light.z, 0.0f);
    write_imagef(output, coord, l);
}
