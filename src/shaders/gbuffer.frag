#version 330 core
layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gAlbedoSpec;

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

void main() {
  // Store the per-fragment normals into the gbuffer - Change [-1..1] to [0..1]
  gNormal = 0.5 * (vec4(1.0, 1.0, 1.0, 0.0) + vec4(normalize(Normal), 0.0f));

  // Store position
  gPosition = vec4(FragPos, 0);

  // Store albedo & specular
  gAlbedoSpec = vec4(material.diffuse, material.specular);

  // Use albedo texture if present
  if (material.tex_count > 0) {
    vec4 texColor = texture(texture_diffuse0, TexCoords).rgba;
    // handle transparency
    if (texColor.a < 0.1)
      discard;
    gAlbedoSpec.rgb *= texColor.rgb;
  }
}
