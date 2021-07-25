// https://learnopengl.com/PBR/Lighting
// To do: Play around with this more to see what looks good.

#include "Common.hlsli"
#include "PSCB0_Scene.hlsli"
#include "PS_Shadow.hlsli"

static const float PI = 3.141592654;
static const float3 F0Const = float3(0.04, 0.04, 0.04);

cbuffer materialConst : register (b1)
{
	float3 mat_base_color;
	float  mat_specular;
	float  mat_metallic;
	float  mat_roughness;
	float  mat_ambient;
};

float3 fresnelSchlick(float cosTheta, float3 F0)
{
	return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}
float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

float3 LightCalc(float3 N, float3 V, float3 F0, float3 L, float3 H, float3 albedo, float3 radiance)
{
    // cook-torrance brdf
    float NDF = DistributionGGX(N, H, mat_roughness);
    float G = GeometrySmith(N, V, L, mat_roughness);
    float3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

    float3 kS = F;
    float3 kD = float3(1.0, 1.0, 1.0) - kS;
    kD *= 1.0 - mat_metallic;

    float3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    float3 specular = numerator / max(denominator, 0.001);

    // add to outgoing radiance Lo
    float NdotL = max(dot(N, L), 0.0);
    return (kD * albedo / PI + specular) * radiance * NdotL;
}

float3 SceneLighting(float3 WorldPos, float3 Normal, float3 albedo)
{
    float3 N = normalize(Normal);
    float3 V = normalize(cameraPosition.xyz - WorldPos);

    float3 F0 = F0Const;
    F0 = lerp(F0, albedo, mat_metallic);

    // reflectance equation
    float3 Lo = float3(0.0, 0.0, 0.0);

    // direcitonal light - sun
    {
        float3 L = normalize(-shadowLightDirection.xyz);
        float3 H = normalize(V + L);
        float3 radiance = shadowLightColor.xyz;
        Lo += LightCalc(N, V, F0, L, H, albedo, radiance);
    }

    // point lights
    for (uint i = 0; i < numberOfPointLights; ++i)
    {
        // calculate per-light radiance
        float3 L = normalize(pointLights[i].position - WorldPos);
        float3 H = normalize(V + L);
        float distance = length(pointLights[i].position - WorldPos);
        float attenuation = 1.0 / (distance * distance);
        float3 radiance = pointLights[i].color * attenuation;
        Lo += LightCalc(N, V, F0, L, H, albedo, radiance);
    }

    float3 ambient = float3(0.03, 0.03, 0.03) * albedo * mat_ambient;
    float3 color = ambient + Lo;

    color = color / (color + float3(1.0, 1.0, 1.0));
    
    color = pow(color, 1.0 / 2.2);

    return color;
}



float4 main(float3 posWS : WSPosition, float3 norWS : WSNormal) : SV_TARGET
{
	return float4(0.0, 0.0, 0.0, 1.0);
}

float4 mainPositionNormal(float3 posWS : WSPosition, float3 norWS : WSNormal) : SV_TARGET
{
	return float4(SceneLighting(posWS, norWS, mat_base_color), 1.0);
}
