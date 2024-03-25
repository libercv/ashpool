#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
  vec4 worldPos = model * vec4(position, 1.0f);
  mat3 normalMatrix = transpose(inverse(mat3(model)));

  FragPos = worldPos.xyz;
  TexCoords = texCoords;
  Normal = normalMatrix * normal;

  gl_Position = projection * view * worldPos;
}
