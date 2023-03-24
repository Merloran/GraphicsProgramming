#version 430 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VSOut
{
	vec3 Normal;
} gsIn[];


void GenerateLine(int index)
{
	gl_Position = gl_in[index].gl_Position;
	EmitVertex();
	gl_Position = gl_in[index].gl_Position + vec4(gsIn[index].Normal, 0.0) * 0.4f;
	EmitVertex();
	EndPrimitive();
}

void main()
{
	GenerateLine(0); // first vertex normal
	GenerateLine(1); // second vertex normal
	GenerateLine(2); // third vertex normal
}  