#version 430 core
out vec4 FragColor;

layout (location = 0) in VSOut
{
	vec2 TexCoords;
	vec3 FragPos;
	vec3 Normal;
	vec4 FragPosLightSpace;
} fsIn;

uniform sampler2D aTexture;

void main()
{
	FragColor = texture(aTexture, fsIn.TexCoords);
} 