#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

layout (std140) uniform Matrixes
{
	uniform mat4 view;			//64
	uniform mat4 projection;	//64
};
uniform mat4 model;
uniform mat4 lightSpace;


layout (location = 0) out VSOut
{
	vec2 TexCoords;
	vec3 WorldPos;
	vec3 Normal;
	vec4 WorldPosLightSpace;
} vsOut;

void main()
{
    vsOut.TexCoords = aTexCoords;
    vsOut.WorldPos = vec3(model * vec4(aPos, 1.0));
    vsOut.Normal = mat3(model) * aNormal;   

    gl_Position =  projection * view * vec4(vsOut.WorldPos, 1.0);
    vsOut.WorldPosLightSpace = lightSpace * vec4(vsOut.WorldPos, 1.0f);
}