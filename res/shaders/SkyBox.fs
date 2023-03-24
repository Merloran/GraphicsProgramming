#version 430 core
in vec3 TexCoords;

uniform samplerCube skybox;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;


void main()
{    
	FragColor = textureLod(skybox, TexCoords, 0.0);
	
    float brightness = dot(FragColor.rgb, vec3(0.2126f, 0.7152f, 0.0722f));
	BrightColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}