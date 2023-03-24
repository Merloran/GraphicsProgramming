#version 330 core
in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

#define NR_DIFFUSE_TEXTURES 4
#define NR_SPECULAR_TEXTURES 4

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

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcReflective(vec3 normal, vec3 I);
 
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform DirLight dirLight;
uniform SpotLight spotLight;

uniform Material material;
uniform vec3 viewPos;
uniform samplerCube skybox;

void main()
{
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
	
	// glass ratio
	//float ratio = 1.00f / 2.42f;
	//vec3 R = refract(-viewDir, norm, ratio);
	
	// Phase 1: Direction light
	vec3 result = CalcDirLight(dirLight, norm, viewDir);
	
	// Phase 2: Point lights
	for(int i = 0; i < NR_POINT_LIGHTS; ++i)
	{
		result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
	}
		
	// Phase 3: Spotlight
	result += CalcSpotLight(spotLight, norm, FragPos, viewDir);
	
	// Phase 4: Enviorment mapping
	result += CalcReflective(norm, -viewDir);
	
	FragColor = vec4(result, 1.0f);
}


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);
	
	// Diffuse parameter
	float diff = max(dot(normal, lightDir), 0.0f);
	
	// Specular parameters
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
	
	// Calculate all lights
	vec3 ambient  = light.ambient  * vec3(texture(material.diffuse[0], TexCoords));
	vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse[0], TexCoords));
	vec3 specular = light.specular * spec * vec3(texture(material.specular[0], TexCoords));
	
	return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
	
	// Diffuse parameter
	float diff = max(dot(normal, lightDir), 0.0f);
	
	// Specular parameters
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
	
	// Calculate attenuation of light in distance
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	
	// Calculate all lights
	vec3 ambient  = light.ambient  * vec3(texture(material.diffuse[0], TexCoords));
	vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse[0], TexCoords));
	vec3 specular = light.specular * spec * vec3(texture(material.specular[0], TexCoords));
	
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
		vec3 lightDir = normalize(light.position - FragPos);
		
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
		vec3 reflectDir = reflect(-lightDir, normal);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
		
		
		// Calculate all lights
		vec3 ambient  = light.ambient  * texture(material.diffuse[0], TexCoords).rgb;
		vec3 diffuse  = light.diffuse  * diff * texture(material.diffuse[0], TexCoords).rgb;  
		vec3 specular = light.specular * spec * texture(material.specular[0], TexCoords).rgb;
		
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
	
	return texture(skybox, R).rgb * texture(material.reflective[0], TexCoords).rgb;
}