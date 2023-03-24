#version 430 core
out vec4 FragColor;

in vec2 TexCoords;

uniform float gamma;
uniform sampler2D screenTexture;
uniform sampler2D bloomBlur;
uniform float exposure;
uniform float bloomStrength = 0.5f;


void main()
{
    vec3 hdrColor = texture(screenTexture, TexCoords).rgb;      
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
	vec3 result;
	if (bloomStrength < 1.0f)
	{
		result = mix(hdrColor, bloomColor, bloomStrength);
	}
	else
	{
		result = hdrColor + bloomColor;
	}
    // tone mapping
    result = vec3(1.0f) - exp(-result * exposure);
    // also gamma correct while we're at it       
    result = pow(result, vec3(1.0f / gamma));
    FragColor = vec4(result, 1.0f);
}