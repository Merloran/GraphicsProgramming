#version 430 core
layout (location = 0) in vec3 aPos;

layout (std140) uniform Matrixes
{
	uniform mat4 view;			//64
	uniform mat4 projection;	//64
};

out vec3 TexCoords;

void main()
{
	TexCoords = aPos;
	mat4 rotView = mat4(mat3(view));
	vec4 result = projection * rotView * vec4(aPos, 1.0);
	gl_Position = result.xyww;
}  