#version 330 core
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D texture_diffuse0;
uniform sampler2D texture_specular0;

void main()
{    
    // Store the fragment position vector in the first gbuffer texture
    gPosition = vec4(FragPos, 0.0f);
    // Also store the per-fragment normals into the gbuffer
    gNormal = vec4(normalize(Normal), 0.0f);
    // And the diffuse per-fragment color
    gAlbedoSpec.rgb = texture(texture_diffuse0, TexCoords).rgb;
    // Store specular intensity in gAlbedoSpec's alpha component
    gAlbedoSpec.a = texture(texture_specular0, TexCoords).r;
}
