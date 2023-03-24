#version 430 core
layout (location = 0) in FSIn
{
	vec2 TexCoords;
	vec3 WorldPos;
	vec3 Normal;
	vec4 WorldPosLightSpace;
} fsIn;

const int MAX_MATERIAL_MAPS_COUNT = 1;

// material parameters
struct Material
{
	sampler2D albedo[MAX_MATERIAL_MAPS_COUNT];
	sampler2D normal[MAX_MATERIAL_MAPS_COUNT];
	sampler2D emission[MAX_MATERIAL_MAPS_COUNT];
	sampler2D metalness[MAX_MATERIAL_MAPS_COUNT];
	sampler2D roughness[MAX_MATERIAL_MAPS_COUNT];
	sampler2D ambientocclusion[MAX_MATERIAL_MAPS_COUNT];
};

struct DirLight
{
	bool isOn;
	
	vec3 direction;
	vec3 color;
};

struct PointLight
{    
	bool isOn;
	
	vec3 position;
	vec3 color;
	
	float constant;
	float linear;
	float quadratic;  
};

struct SpotLight
{
	bool isOn;
	
	vec3 position;
	vec3 direction;
	vec3 color;
	
	float cutOff;
	float outerCutOff;

	float constant;
	float linear;
	float quadratic;  

};  

// IBL
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;
uniform sampler2D shadowMap;

// lights
uniform PointLight pointLights[4];
uniform SpotLight spotLights[2];
uniform DirLight dirLights[1];

uniform Material material;

uniform bool isRefract;
uniform vec3 camPos;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

const float PI = 3.14159265359;
// ----------------------------------------------------------------------------
// Easy trick to get tangent-normals to world-space to keep PBR code simplified.
// Don't worry if you don't get what's going on; you generally want to do normal 
// mapping the usual way for performance anways; I do plan make a note of this 
// technique somewhere later in the normal mapping tutorial.
vec3 getNormalFromMap(vec3 worldPos, vec2 texCoords, vec3 normal, sampler2D normalMap);
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 F0, float roughness, float metalness, vec3 albedo, vec4 worldPosLightSpace);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 worldPos, vec3 viewDir, vec3 F0, float roughness, float metalness, vec3 albedo);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 worldPos, vec3 viewDir, vec3 F0, float roughness, float metalness, vec3 albedo);
float ShadowCalculation(vec4 worldPosLightSpace, vec3 normal, vec3 lightDir);

void main()
{		
    // material properties
	vec4 tempAlbedo = texture(material.albedo[0], fsIn.TexCoords);
	if (tempAlbedo.a < 0.15f)
	{
		discard;
	}
    vec3 albedo = pow(tempAlbedo.rgb, vec3(2.2));
    float metalness = texture(material.metalness[0], fsIn.TexCoords).r;
    float roughness = texture(material.roughness[0], fsIn.TexCoords).r;
    float ao = texture(material.ambientocclusion[0], fsIn.TexCoords).r;
    vec3 emission = texture(material.emission[0], fsIn.TexCoords).rgb;
       
    // input lighting data
    vec3 normal = getNormalFromMap(fsIn.WorldPos, fsIn.TexCoords, fsIn.Normal, material.normal[0]);
	// View position
    vec3 viewDir = normalize(camPos - fsIn.WorldPos);
    vec3 refl = reflect(-viewDir, normal); 

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metalness);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < 4; ++i) 
    {
		Lo += CalcPointLight(pointLights[i], normal, fsIn.WorldPos, viewDir, F0, roughness, metalness, albedo);
    }
    for(int i = 0; i < 2; ++i) 
    {
		Lo += CalcSpotLight(spotLights[i], normal, fsIn.WorldPos, viewDir, F0, roughness, metalness, albedo);
	}
    Lo += CalcDirLight(dirLights[0], normal, viewDir, F0, roughness, metalness, albedo, fsIn.WorldPosLightSpace);
    // ambient lighting (we now use IBL as the ambient term)
    vec3 F = fresnelSchlickRoughness(max(dot(normal, viewDir), 0.0), F0, roughness);
    
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metalness;	  
    
    vec3 irradiance = texture(irradianceMap, normal).rgb;
    vec3 diffuse    = irradiance * albedo;
    
    // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, refl,  roughness * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = texture(brdfLUT, vec2(max(dot(normal, viewDir), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

    vec3 ambient = (kD * diffuse + specular) * ao;
    
    vec3 color = ambient + Lo + emission * 5.0f;
	
	if (isRefract)
	{
		float ratio = 1.00 / 1.52;
		vec3 I = normalize(fsIn.WorldPos - camPos);
		vec3 R = refract(I, normal, ratio);
		FragColor = vec4(texture(prefilterMap, R).rgb, 1.0);
	}
	else
	{
		FragColor = vec4(color , 1.0);
	}
	
    float brightness = dot(color, vec3(0.2126f, 0.7152f, 0.0722f));
    if(brightness > 1.0f)
	{
        BrightColor = vec4(color, 1.0f);
	}
    else
	{
        BrightColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}
}


// ----------------------------------------------------------------------------
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 F0, float roughness, float metalness, vec3 albedo, vec4 worldPosLightSpace)
{
	if (!light.isOn)
	{
		return vec3(0.0f);
	}
	
	vec3 lightDir = normalize(-light.direction);
    vec3 halfwayDir = normalize(lightDir + viewDir);

	// Cook-Torrance BRDF
	float NDF = DistributionGGX(normal, halfwayDir, roughness);   
	float G   = GeometrySmith(normal, viewDir, lightDir, roughness);    
	vec3  F   = fresnelSchlick(max(dot(halfwayDir, viewDir), 0.0), F0);        
	
	// scale light by NdotL
	float NdotL = max(dot(normal, lightDir), 0.0);    
	
	vec3 numerator    = NDF * G * F;
	float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * NdotL + 0.0001; // + 0.0001 to prevent divide by zero
	vec3 specular = numerator / denominator;
	
	 // kS is equal to Fresnel
	vec3 kS = F;
	// for energy conservation, the diffuse and specular light can't
	// be above 1.0 (unless the surface emits light); to preserve this
	// relationship the diffuse component (kD) should equal 1.0 - kS.
	vec3 kD = vec3(1.0) - kS;
	// multiply kD by the inverse metalness such that only non-metals 
	// have diffuse lighting, or a linear blend if partly metal (pure metals
	// have no diffuse light).
	kD *= 1.0 - metalness;
	
    float shadow = ShadowCalculation(worldPosLightSpace, normal,  lightDir);   
	
	// note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
	return (kD * albedo / PI + specular) * (1.0f - shadow) * light.color * NdotL;

}

// ----------------------------------------------------------------------------
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 worldPos, vec3 viewDir, vec3 F0, float roughness, float metalness, vec3 albedo)
{
	if (!light.isOn)
	{
		return vec3(0.0f);
	}
	
	// calculate per-light radiance
	vec3 lightDir = normalize(light.position - worldPos);
	vec3 halfwayDir = normalize(viewDir + lightDir);
	
	
	// Calculate attenuation of light in distance
	float distance = length(light.position - worldPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	
	vec3 radiance = light.color * attenuation;

	// Cook-Torrance BRDF
	float NDF = DistributionGGX(normal, halfwayDir, roughness);   
	float G   = GeometrySmith(normal, viewDir, lightDir, roughness);    
	vec3  F   = fresnelSchlick(max(dot(halfwayDir, viewDir), 0.0), F0);        
	
	
	// scale light by NdotL
	float NdotL = max(dot(normal, lightDir), 0.0);    
	
	vec3 numerator    = NDF * G * F;
	float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * NdotL + 0.0001; // + 0.0001 to prevent divide by zero
	vec3 specular = numerator / denominator;
	
	 // kS is equal to Fresnel
	vec3 kS = F;
	// for energy conservation, the diffuse and specular light can't
	// be above 1.0 (unless the surface emits light); to preserve this
	// relationship the diffuse component (kD) should equal 1.0 - kS.
	vec3 kD = vec3(1.0) - kS;
	// multiply kD by the inverse metalness such that only non-metals 
	// have diffuse lighting, or a linear blend if partly metal (pure metals
	// have no diffuse light).
	kD *= 1.0 - metalness;
	
	// note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
	return (kD * albedo / PI + specular) * radiance * NdotL;
}

// ----------------------------------------------------------------------------
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 worldPos, vec3 viewDir, vec3 F0, float roughness, float metalness, vec3 albedo)
{
	if (!light.isOn)
	{
		return vec3(0.0f);
	}
	
	vec3 lightDir = normalize(light.position - worldPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	
	// Calculate intensity of light on egdes
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0f, 1.0f);
	
	
	// Calculate attenuation of light in distance
	float distance = length(light.position - worldPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));  
	
	
	vec3 radiance = light.color * attenuation * intensity;

	// Cook-Torrance BRDF
	float NDF = DistributionGGX(normal, halfwayDir, roughness);   
	float G   = GeometrySmith(normal, viewDir, lightDir, roughness);    
	vec3  F   = fresnelSchlick(max(dot(halfwayDir, viewDir), 0.0), F0);        
	
	
	// scale light by NdotL
	float NdotL = max(dot(normal, lightDir), 0.0);    
	
	vec3  numerator   = NDF * G * F;
	float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * NdotL + 0.0001; // + 0.0001 to prevent divide by zero
	vec3  specular    = numerator / denominator;
	
	 // kS is equal to Fresnel
	vec3 kS = F;
	// for energy conservation, the diffuse and specular light can't
	// be above 1.0 (unless the surface emits light); to preserve this
	// relationship the diffuse component (kD) should equal 1.0 - kS.
	vec3 kD = vec3(1.0) - kS;
	// multiply kD by the inverse metalness such that only non-metals 
	// have diffuse lighting, or a linear blend if partly metal (pure metals
	// have no diffuse light).
	kD *= 1.0 - metalness;
	
	// note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
	return (kD * albedo / PI + specular) * radiance * NdotL;
}

// ----------------------------------------------------------------------------
float ShadowCalculation(vec4 worldPosLightSpace, vec3 normal, vec3 lightDir)
{
    // perform perspective divide
    vec3 projCoords = worldPosLightSpace.xyz / worldPosLightSpace.w;
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

// ----------------------------------------------------------------------------
vec3 getNormalFromMap(vec3 worldPos, vec2 texCoords, vec3 normal, sampler2D normalMap)
{
    vec3 tangentNormal = texture(normalMap, texCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(worldPos);
    vec3 Q2  = dFdy(worldPos);
    vec2 st1 = dFdx(texCoords);
    vec2 st2 = dFdy(texCoords);

    vec3 N   = normalize(normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// ----------------------------------------------------------------------------
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}   