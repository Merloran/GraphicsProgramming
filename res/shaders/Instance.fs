#version 430 core
layout (location = 0) in FSIn
{
	vec2 TexCoords;
} fsIn;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

#define NR_DIFFUSE_TEXTURES 1

struct Material
{
	sampler2D diffuse[NR_DIFFUSE_TEXTURES];
}; 
 
uniform Material material;

void main()
{
	FragColor = texture(material.diffuse[0], fsIn.TexCoords);
	BrightColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}