#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VSOut
{
	vec2 TexCoords;
	vec4 FragPosLightSpace;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} gsIn[];

out vec2 TexCoords;
out vec4 FragPosLightSpace;
out vec3 TangentLightPos;
out vec3 TangentViewPos;
out vec3 TangentFragPos;

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
		TexCoords		  = gsIn[i].TexCoords;
		FragPosLightSpace = gsIn[i].FragPosLightSpace;
		TangentLightPos   = gsIn[i].TangentLightPos;
		TangentViewPos 	  = gsIn[i].TangentViewPos;
		TangentFragPos    = gsIn[i].TangentFragPos;
		gl_Position = explode(gl_in[i].gl_Position, normal);
		EmitVertex();
	}
	EndPrimitive();
}  