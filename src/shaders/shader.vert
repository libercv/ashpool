#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out vec2 TexCoords;
out vec3 Normal;

//uniform mat4 model;
//uniform mat4 view;
//uniform mat4 projection;
uniform mat4 mv;
uniform mat4 mvp;

void main()
{
	//gl_Position = projection * view * model * vec4(position, 1.0f);
	gl_Position = mvp * vec4(position, 1.0f);
	Normal = normalize(vec3(mv * vec4(normal,0.0)));
	//TexCoords=vec2(texCoords.x, 1-texCoords.y);
	TexCoords=texCoords;
}
