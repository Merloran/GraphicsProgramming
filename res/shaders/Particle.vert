#version 460 core
layout (location = 0) in vec4 Position;


layout (std140) uniform Matrixes
{
	uniform mat4 view;			//64
	uniform mat4 projection;	//64
};
uniform mat4 model;

void main()
{
    gl_PointSize = 10.0;
	gl_Position = projection * view * model * Position;
}