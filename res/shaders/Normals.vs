#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

layout (std140) uniform Matrixes
{
	uniform mat4 view;			//64
	uniform mat4 projection;	//64
};
uniform mat4 model;

out VSOut
{
	vec3 Normal;
} vsOut;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	
	mat3 normalMatrix = mat3(transpose(inverse(view * model)));
	vsOut.Normal = normalize(vec3(projection * vec4(normalMatrix * aNormal, 0.0)));
}