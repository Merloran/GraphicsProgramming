#version 430 core
layout (location = 0) in vec3  aPos;
layout (location = 1) in vec3  aNormal;
layout (location = 2) in vec2  aTexCoords;
layout (location = 4) in ivec4 skinIndices; 
layout (location = 5) in vec4  skinWeights;

layout (std140) uniform Matrixes
{
	uniform mat4 view;			//64
	uniform mat4 projection;	//64
};
uniform mat4 model;
uniform mat4 lightSpace;

const int MAX_BONES = 512;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

layout (location = 0) out VSOut
{
	vec2 TexCoords;
	vec3 WorldPos;
	vec3 Normal;
	vec4 WorldPosLightSpace;
} vsOut;

void main()
{
    const vec4 pos = vec4(aPos, 1.0f);
    const vec4 norm = vec4(aNormal, 0.0f);
    vec4 posSkinned = vec4(0.0f);
    vec4 normSkinned = vec4(0.0f);

    for(int i = 0; i < MAX_BONE_INFLUENCE; ++i)
    {
        if(skinWeights[i] > 0.0f && skinIndices[i] < MAX_BONES)
        {
            const mat4 bone = finalBonesMatrices[skinIndices[i]];
            const float weight = skinWeights[i];
			
            posSkinned += (bone * pos) * weight;
            normSkinned += (bone * norm) * weight;
        }
    }

	vsOut.TexCoords = aTexCoords;
	vsOut.WorldPos = vec3(model * posSkinned);
	vsOut.Normal = vec3(model * normSkinned);   

	gl_Position =  projection * view * model * posSkinned;
	vsOut.WorldPosLightSpace = lightSpace * model * posSkinned;
}