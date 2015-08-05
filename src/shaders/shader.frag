#version 330 core

in vec2 TexCoords;
in vec3 Normal;
out vec4 outColor;


uniform sampler2D texture_diffuse1;
uniform int material_texCount;
uniform vec4 material_diffuse;
uniform vec4 material_ambient;


void main()
{   	
    vec4 color;
    vec4 amb;
    float intensity;
    vec3 lightDir;
    vec3 n;
 
    lightDir = normalize(vec3(1.0,1.0,1.0));
    n = normalize(Normal);
    intensity = max(dot(lightDir,n),0.0);
 
    if (material_texCount == 0) {
        color = material_diffuse;
        amb = material_ambient;
    }
    else {
	color = vec4(texture(texture_diffuse1, TexCoords));
        amb = color * 0.33;
    }
    outColor = (color * intensity) + amb;
}



