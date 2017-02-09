#version 330 core
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in mat3 TBN;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
  
uniform Material material;
uniform sampler2D texture_diffuse0;
uniform sampler2D texture_specular0;
uniform sampler2D texture_normal0;
uniform bool options_normal_mapping;


void main()
{    
    // Store the fragment position vector in the first gbuffer texture
    gPosition = vec4(FragPos, 0.0f);
        
    // Also store the per-fragment normals into the gbuffer
    gNormal = vec4(normalize(Normal), 0.0f);
    if (options_normal_mapping) {
      /*
      vec3 normal = texture(texture_normal0, TexCoords).xyz;
      //normal = normalize(normal * 2.0 - 1.0);   
      normal = normalize(TBN * normal);           
      gNormal = vec4(normal, 0.0f);
      */
      vec3 height_m = normalize(texture(texture_normal0, TexCoords).xyz);
      float height=height_m.x * 2.0 - 1.0;
      gPosition = vec4(FragPos, 0.0f) + gNormal*height;
    }
      
    
    // And the diffuse per-fragment color
    if (material.diffuse.x!=0)
      gAlbedoSpec.rgb =  material.diffuse*texture(texture_diffuse0, TexCoords).rgb;
    else
      gAlbedoSpec.rgb =  texture(texture_diffuse0, TexCoords).rgb;
    // Store specular intensity in gAlbedoSpec's alpha component
    //gAlbedoSpec.a = texture(texture_specular0, TexCoords).r;
    gAlbedoSpec.a = material.specular.r;
}
