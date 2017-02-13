#version 330 core
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    float specular;
    float shininess;
    int tex_count;
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
        
    // Store the per-fragment normals into the gbuffer
    gNormal = vec4(normalize(Normal), 0.0f);
    
    // Store albedo & specular
    gAlbedoSpec = vec4( material.diffuse, material.specular);
    
    // Use albedo texture if present
    if (material.tex_count>0)
      gAlbedoSpec.rgb *= texture(texture_diffuse0, TexCoords).rgb;      
}
