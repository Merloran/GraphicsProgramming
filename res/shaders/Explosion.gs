#version 430 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

layout (location = 0) in VSOut
{
	vec2 TexCoords;
	vec3 FragPos;
	vec3 Normal;
	vec4 FragPosLightSpace;
} gsIn[];

layout (location = 0) out GSOut
{
	vec2 TexCoords;
	vec3 FragPos;
	vec3 Normal;
	vec4 FragPosLightSpace;
} gsOut;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;
out vec4 FragPosLightSpace;

uniform float time;

vec3 GetNormal()
{
   vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
   vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
   return normalize(cross(a, b));
}

vec4 explode(vec4 position, vec3 normal)
{
	vec3 direction = normal * time; 
	return position + vec4(direction, 0.0);
}


void main()
{
	vec3 normal = GetNormal();
	for (int i = 0; i < 3; ++i) // Remove loop to optimize
	{
		gsOut.TexCoords = gsIn[i].TexCoords;
		gsOut.Normal = gsIn[i].Normal;
		gsOut.FragPos = gsIn[i].FragPos;
		gsOut.FragPosLightSpace = gsIn[i].FragPosLightSpace;
		gl_Position = explode(gl_in[i].gl_Position, normal);
		EmitVertex();
	}
	EndPrimitive();
}