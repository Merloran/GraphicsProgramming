#version 430 core

layout (location = 0) in FSIn
{
	vec2 TexCoords;
	vec3 FragPos;
	vec3 Normal;
	vec4 FragPosLightSpace;
} fsIn;

uniform samplerCube skybox;
uniform vec3 camPos;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor; 

void main()
{
	float ratio = 1.00 / 1.52;
	vec3 I = normalize(fsIn.FragPos - camPos);
	vec3 R = refract(I, normalize(fsIn.Normal), ratio);
	FragColor = vec4(texture(skybox, R).rgb, 1.0);
	BrightColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}  