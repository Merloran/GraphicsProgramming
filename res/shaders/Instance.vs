#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 instanceModel;

layout (std140) uniform Matrixes
{
	uniform mat4 view;			//64
	uniform mat4 projection;	//64
};
uniform mat4 model;

layout (location = 0) out VSOut
{
	vec2 TexCoords;
} vsOut;

void main()
{
	gl_Position = projection * view * model * instanceModel * vec4(aPos, 1.0f);
	vsOut.TexCoords = aTexCoords;
}