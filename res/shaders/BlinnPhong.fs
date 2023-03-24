#version 430 core
#define NR_DIFFUSE_TEXTURES 4
#define NR_SPECULAR_TEXTURES 4 
#define NR_POINT_LIGHTS 4


struct Material
{
	sampler2D diffuse[NR_DIFFUSE_TEXTURES];
	sampler2D specular[NR_SPECULAR_TEXTURES];
	sampler2D reflective[NR_SPECULAR_TEXTURES];
	
	float shininess;
}; 

struct PointLight
{    
	vec3 position;

	float constant;
	float linear;
	float quadratic;  

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};  

struct DirLight
{
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct SpotLight
{
	bool isOn;
	
	vec3 position;
	vec3 direction;
	
	float cutOff;
	float outerCutOff;

	float constant;
	float linear;
	float quadratic;  

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};  

layout (location = 0) in FSIn
{
	vec2 TexCoords;
	vec3 FragPos;
	vec3 Normal;
	vec4 FragPosLightSpace;
} fsIn;

uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform DirLight dirLight;
uniform SpotLight spotLight;
uniform sampler2D shadowMap;

uniform Material material;
uniform vec3 viewPos;
uniform samplerCube skybox;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor; 

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir);
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcReflective(vec3 normal, vec3 I);

void main()
{
	vec3 norm = normalize(fsIn.Normal);
	vec3 viewDir = normalize(viewPos - fsIn.FragPos);
	
	// glass ratio
	//float ratio = 1.00f / 2.42f;
	//vec3 R = refract(-viewDir, norm, ratio);
	
	// Phase 1: Direction light
	vec3 result = CalcDirLight(dirLight, norm, viewDir);
	
	// Phase 2: Point lights
	for(int i = 0; i < NR_POINT_LIGHTS; ++i)
	{
		result += CalcPointLight(pointLights[i], norm, fsIn.FragPos, viewDir);
	}
	
	// Phase 3: Spotlight
	result += CalcSpotLight(spotLight, norm, fsIn.FragPos, viewDir);
	
	// Phase 4: Environment mapping
	result += CalcReflective(norm, -viewDir);
	
	
    float brightness = dot(result, vec3(0.2126f, 0.7152f, 0.0722f));
    if(brightness > 1.0f)
	{
        BrightColor = vec4(result, 1.0f);
	}
    else
	{
        BrightColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}
	FragColor = vec4(result, 1.0f);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);
    vec3 halfwayDir = normalize(lightDir + viewDir);
	
	// Diffuse parameter
	float diff = max(dot(normal, lightDir), 0.0f);
	
	// Specular parameters
	float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
	
	// Calculate all lights
	vec3 ambient  = light.ambient  * vec3(texture(material.diffuse[0], fsIn.TexCoords));
	vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse[0], fsIn.TexCoords));
	vec3 specular = light.specular * spec * vec3(texture(material.specular[0], fsIn.TexCoords));
	
    float shadow = ShadowCalculation(fsIn.FragPosLightSpace, normal,  lightDir);   
	
	return (ambient + (1.0f - shadow) * (diffuse + specular));
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
	
	// Diffuse parameter
	float diff = max(dot(normal, lightDir), 0.0f);
	
	// Specular parameters
	float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
	
	// Calculate attenuation of light in distance
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	
	// Calculate all lights
	vec3 ambient  = light.ambient  * vec3(texture(material.diffuse[0], fsIn.TexCoords));
	vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse[0], fsIn.TexCoords));
	vec3 specular = light.specular * spec * vec3(texture(material.specular[0], fsIn.TexCoords));
	
	// Apply attenuation
	ambient  *= attenuation;
	diffuse  *= attenuation;
	specular *= attenuation;
	
	return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	if (light.isOn)
	{
		vec3 lightDir = normalize(light.position - fragPos);
		vec3 halfwayDir = normalize(lightDir + viewDir);
		
		// Calculate intensity of light on egdes
		float theta = dot(lightDir, normalize(-light.direction));
		float epsilon = light.cutOff - light.outerCutOff;
		float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0f, 1.0f);
		
		// Diffuse parameter
		float diff = max(dot(normal, lightDir), 0.0f);
		
		// Calculate attenuation of light in distance
		float distance = length(light.position - fragPos);
		float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));  
		
		// Specular parameters
		float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
		
		
		// Calculate all lights
		vec3 ambient  = light.ambient  * texture(material.diffuse[0], fsIn.TexCoords).rgb;
		vec3 diffuse  = light.diffuse  * diff * texture(material.diffuse[0], fsIn.TexCoords).rgb;  
		vec3 specular = light.specular * spec * texture(material.specular[0], fsIn.TexCoords).rgb;
		
		// Apply attenuation and intensity
		ambient *= attenuation * intensity;
		diffuse *= attenuation * intensity;
		specular *= attenuation * intensity;
		
		return (ambient + diffuse + specular);
	}
	else
	{
		return vec3(0.0f);
	}
}

vec3 CalcReflective(vec3 normal, vec3 I)
{
	vec3 R = reflect(I, normal);
	
	return texture(skybox, R).rgb * texture(material.reflective[0], fsIn.TexCoords).rgb;
}

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5f + 0.5f;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
	
	float shadow = 0.0f;
	float bias = max(0.05f * (1.0f - dot(normal, lightDir)), 0.005f); 
    vec2 texelSize = 1.0f / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0f : 0.0f;        
        }    
    }
    shadow /= 9.0f;
	
	if(projCoords.z > 1.0f)
	{
        shadow = 0.0f;
	}

    return shadow;
}