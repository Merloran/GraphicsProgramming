#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

layout (location = 0) out VSOut
{
	vec2 TexCoords;
	vec3 FragPos;
	vec3 Normal;
	vec4 FragPosLightSpace;
} vsOut;

layout (std140) uniform Matrixes
{
	uniform mat4 view;			//64
	uniform mat4 projection;	//64
};
uniform mat4 model;
uniform mat4 lightSpaceMatrix;



void main()
{
	vsOut.TexCoords = aTexCoords;
	vsOut.Normal = mat3(transpose(inverse(model))) * aNormal;
	vsOut.FragPos = vec3(model * vec4(aPos, 1.0));
	gl_Position = projection * view * vec4(vsOut.FragPos, 1.0f);
	
    vsOut.FragPosLightSpace = lightSpaceMatrix * vec4(vsOut.FragPos, 1.0f);
}