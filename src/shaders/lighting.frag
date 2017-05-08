#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

struct Light {
  vec3 Position;
  vec3 Color;
  float Linear;
  float Quadratic;
  float Intensity;
};
const int MAX_LIGHTS = 25;
uniform int NR_LIGHTS;
uniform Light lights[MAX_LIGHTS];
uniform vec3 viewPos;
uniform float ambient;

void main() {
  // Retrieve data from gbuffer
  vec3 FragPos = texture(gPosition, TexCoords).rgb;
  vec3 Normal = texture(gNormal, TexCoords).rgb;
  // Change [0..1] to [-1..1]
  Normal = normalize(2 * Normal - vec3(1, 1, 1));
  vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;
  float Specular = texture(gAlbedoSpec, TexCoords).a;
  // Then calculate lighting as usual
  vec3 lighting = Diffuse * ambient;

  vec3 viewDir = normalize(viewPos - FragPos);
  for (int i = 0; i < NR_LIGHTS; ++i) {
    // Diffuse
    vec3 lightDir = normalize(lights[i].Position - FragPos);
    vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lights[i].Color *
                   lights[i].Intensity;
    // Specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
    vec3 specular = lights[i].Color * spec * Specular;
    // Attenuation
    float distance = length(lights[i].Position - FragPos);
    float attenuation = 1.0 / (1.0 + lights[i].Linear * distance +
                               lights[i].Quadratic * distance * distance);
    diffuse *= attenuation;
    specular *= attenuation;
    lighting += diffuse + specular;
  }
  FragColor = vec4(lighting, 1.0);
  // FragColor = vec4(Normal, 1.0);
}
