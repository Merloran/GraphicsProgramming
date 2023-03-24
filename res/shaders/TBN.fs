#version 430 core
in vec2 TexCoords;
in vec4 FragPosLightSpace;
in vec3 TangentLightPos;
in vec3 TangentViewPos;
in vec3 TangentFragPos;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

#define NR_DIFFUSE_TEXTURES 4
#define NR_SPECULAR_TEXTURES 4
#define NR_REFLECTIVE_TEXTURES 1
#define NR_NORMAL_TEXTURES 1

struct Material
{
	sampler2D diffuse[NR_DIFFUSE_TEXTURES];
	sampler2D specular[NR_SPECULAR_TEXTURES];
	sampler2D reflective[NR_REFLECTIVE_TEXTURES];
	sampler2D normal[NR_NORMAL_TEXTURES];
	
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


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec2 texCoords);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec2 texCoords);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec2 texCoords);
vec3 CalcReflective(vec3 normal, vec3 I, vec2 texCoords);
float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir);
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir);
 
#define NR_POINT_LIGHTS 1
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform DirLight dirLight;
uniform SpotLight spotLight;
uniform sampler2D shadowMap;

uniform float heightScale;
uniform Material material;
uniform vec3 viewPos;
uniform samplerCube skybox;

void main()
{
    vec3 viewDir = normalize(TangentViewPos - TangentFragPos);
	
	vec2 tex = ParallaxMapping(TexCoords, viewDir);
	if(tex.x > 1.0 || tex.y > 1.0 || tex.x < 0.0 || tex.y < 0.0)
	{
        discard;
	}
     // obtain normal from normal map in range [0,1]
    vec3 normal = texture(material.normal[0], tex).rgb;
    // transform normal vector to range [-1,1]
    normal = normalize(normal * 2.0f - 1.0f);  // this normal is in tangent space
	
	
	// glass ratio
	//float ratio = 1.00f / 2.42f;
	//vec3 R = refract(-viewDir, norm, ratio);
	
	// Phase 1: Direction light
	vec3 result = CalcDirLight(dirLight, normal, viewDir, tex);
	
	// Phase 2: Point lights
	for(int i = 0; i < NR_POINT_LIGHTS; ++i)
	{
		result += CalcPointLight(pointLights[i], normal, TangentFragPos, viewDir, tex);
	}
	
	// Phase 3: Spotlight
	result += CalcSpotLight(spotLight, normal, TangentFragPos, viewDir, tex);
	
	// Phase 4: Enviorment mapping
	//result += CalcReflective(normal, -viewDir, tex);
	
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


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec2 texCoords)
{
	vec3 lightDir = normalize(-light.direction);
    vec3 halfwayDir = normalize(lightDir + viewDir);
	
	// Diffuse parameter
	float diff = max(dot(normal, lightDir), 0.0f);
	
	// Specular parameters
	float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
	
	// Calculate all lights
	vec3 ambient  = light.ambient  * vec3(texture(material.diffuse[0], texCoords));
	vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse[0], texCoords));
	vec3 specular = light.specular * spec * vec3(texture(material.specular[0], texCoords));
	
    float shadow = ShadowCalculation(FragPosLightSpace, normal,  lightDir);   
	
	return (ambient + (1.0f - shadow) * (diffuse + specular));
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec2 texCoords)
{
	vec3 lightDir = normalize(TangentLightPos - TangentFragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
	
	// Diffuse parameter
	float diff = max(dot(normal, lightDir), 0.0f);
	
	// Specular parameters
	float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
	
	// Calculate attenuation of light in distance
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	
	// Calculate all lights
	vec3 ambient  = light.ambient  * vec3(texture(material.diffuse[0], texCoords));
	vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse[0], texCoords));
	vec3 specular = light.specular * spec * vec3(texture(material.specular[0], texCoords));
	
	// Apply attenuation
	ambient  *= attenuation;
	diffuse  *= attenuation;
	specular *= attenuation;
	
	return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec2 texCoords)
{
	if (light.isOn)
	{
		vec3 lightDir = normalize(TangentViewPos - fragPos);
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
		vec3 ambient  = light.ambient  * texture(material.diffuse[0], texCoords).rgb;
		vec3 diffuse  = light.diffuse  * diff * texture(material.diffuse[0], texCoords).rgb;  
		vec3 specular = light.specular * spec * texture(material.specular[0], texCoords).rgb;
		
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

vec3 CalcReflective(vec3 normal, vec3 I, vec2 texCoords)
{
	vec3 R = reflect(I, normal);
	
	return texture(skybox, R).rgb * texture(material.reflective[0], texCoords).rgb;
}

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
	
	float shadow = 0.0f;
	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005); 
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

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{ 
    // number of depth layers
    const float minLayers = 8;
    const float maxLayers = 128;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  
    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = viewDir.xy / viewDir.z * heightScale; 
    vec2 deltaTexCoords = P / numLayers;
  
    // get initial values
    vec2  currentTexCoords     = texCoords;
    float currentDepthMapValue = texture(material.reflective[0], currentTexCoords).r;
      
    while(currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = texture(material.reflective[0], currentTexCoords).r;  
        // get depth of next layer
        currentLayerDepth += layerDepth;  
    }
    
    // get texture coordinates before collision (reverse operations)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(material.reflective[0], prevTexCoords).r - currentLayerDepth + layerDepth;
 
    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}
