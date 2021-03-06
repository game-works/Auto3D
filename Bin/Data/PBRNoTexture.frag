#version 150

#include "CommonCode.frag"
#include "BRDF.frag"
#include "math.glsl"

in vec4 vWorldPos;
in vec3 vNormal;
in vec2 vTexCoord;

out vec4 FragColor;    

layout(std140) uniform material
{
	vec3 albedo;
	float metallic;
	float roughness;
	float ao;
};

// lights
uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];

uniform vec3 camPos; 

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

    return nom / max(denom, 0.001); 
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
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
// ----------------------------------------------------------------------------
void main()
{		
    vec3 N = normalize(Normal);	
    vec3 V = normalize(camPos - WorldPos); 


	vec3 F0 = vec3(0.04); 
	F0 = mix(F0, albedo, metallic);

	vec3 Lo = vec3(0.0);
	for(int i = 0; i < 4; ++i)			
	{
		// calculate per-light radiance
		vec3 L = normalize(lightPositions[i] - WorldPos);	 
		vec3 H = normalize(V + L);							
		float distance = length(lightPositions[i] - WorldPos);	
		float attenuation = 1.0 / (distance * distance);
		vec3 radiance = lightColors[i] * attenuation;

		// Cook-Torrance BRDF 
		float NDF = DistributionGGX(N, H, roughness);
		float G   = GeometrySmith(N, V, L, roughness);
		vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
           
		vec3 nominator    = NDF * G * F;
		float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
		vec3 specular = nominator / max(denominator, 0.001);

		vec3 kS = F;
		
		vec3 kD = vec3(1.0) - kS;

		kD *= 1.0 - metallic;	  

		// scale light by NdotL
		float NdotL = max(dot(N, L), 0.0);        

	
		Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
	}   

    vec3 ambient = vec3(0.03) * albedo * ao;

    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color, 1.0);
}
