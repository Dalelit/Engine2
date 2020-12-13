#include "PS_Shadow.hlsli"
#include "PS_Lighting.hlsli"
#include "PSCB0_Scene.hlsli"
#include "PSCB1_Material.hlsli"

Texture2D tex : register (t2);
SamplerState smplr : register (s2);

float4 main(float3 posWS : WSPosition, float3 norWS : WSNormal, float2 tc : Texcoord) : SV_TARGET
{
	float4 target = 0.0;

	float4 col = float4(tex.Sample(smplr, tc).rgb, 1.0f);
	//float4 col = float4(tc.x, tc.y, 0.0, 1.0);

	target += ShadowLighting(posWS);

	norWS = normalize(norWS);
	float3 toLight = pointLightPosition.xyz - posWS.xyz;
	float norDotLight = dot(norWS, normalize(toLight));

	// If facing the light
	if (norDotLight > 0.0)
	{
		const float PI = 3.14159565359;

		// calc the light
		float att = attenuation(length(toLight)) * norDotLight;
		att /= PI;
		target += att; // defer *col, defer *pointLightColor

		// calc the specular light strength, then add it to the pixel with the specular color
		float specStrength = specular(posWS.xyz, norWS.xyz, pointLightPosition.xyz, cameraPosition.xyz, mat_specularExponent);
		target += specStrength * float4(mat_specular, 1.0); // defer *col, defer *pointLightColor

		target *= pointLightColor;
	}

	// Add ambient
	target += float4(mat_ambient, 1.0); // defer *col;

	target *= col; // do the deferred *col

	target += float4(mat_emission, 1.0);

	return target;
}
