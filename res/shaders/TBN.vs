#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

layout (std140) uniform Matrixes
{
	uniform mat4 view;			//64
	uniform mat4 projection;	//64
};
uniform mat4 model;

uniform mat4 lightSpaceMatrix;

uniform vec3 lightPos;
uniform vec3 viewPos;

out VSOut
{
	vec2 TexCoords;
	vec4 FragPosLightSpace;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} vsOut;

void main()
{
	vsOut.TexCoords = aTexCoords;
	vec3 FragPos = vec3(model * vec4(aPos, 1.0));
	
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    
    mat3 TBN = transpose(mat3(T, B, N));
	
	// front face fix
	if (TBN == mat3(1.0f))
	{
		TBN = transpose(mat3(T, -B, N));
	}
	
    vsOut.TangentLightPos = TBN * lightPos;
    vsOut.TangentViewPos  = TBN * viewPos;
    vsOut.TangentFragPos  = TBN * FragPos;
	
	gl_Position = projection * view * vec4(FragPos, 1.0f);
	
	
    vsOut.FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0f);
}