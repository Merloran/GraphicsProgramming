#version 430 core
layout (location = 0) in vec3 aPos;

uniform mat4 view;			//64
uniform mat4 projection;	//64

out vec3 WorldPos;

void main()
{
    WorldPos = aPos;  
    gl_Position =  projection * view * vec4(WorldPos, 1.0);
}