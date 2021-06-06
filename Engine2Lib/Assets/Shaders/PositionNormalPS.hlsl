#include "Common.hlsli"
#include "PSCB0_Scene.hlsli"
#include "PSCB1_Material.hlsli"
#include "PS_Shadow.hlsli"
#include "PS_Lighting.hlsli"

Texture2D tex : register (t2);
SamplerState smplr : register (s2);


float4 processPixel(float3 materialColor, float3 posWS, float3 norWS)
{
	float4 target = 0.0;

	norWS = normalize(norWS);

	// do the sun with shadows
	target += ShadowLighting(posWS, norWS) * float4(mat_diffuse, 1.0);

	// do specular off the sun if not in shadow
	if (length(target.xyz) > 0.0)
	{
		target += target * specular(posWS.xyz, norWS.xyz, shadowLightDirection.xyz, cameraPosition.xyz, mat_specularExponent) * float4(mat_specular * materialColor, 1.0);
	}

	// Add each point light
	for (uint i = 0; i < numberOfPointLights; i++)
	{
		pointLightData pl = pointLights[i];
		
		float3 toLight = pl.position - posWS.xyz;
		float dist = length(toLight);
		toLight = normalize(toLight);
		float dotNorLight = dot(norWS, toLight);

		// if facing the light
		if (dotNorLight > 0.0)
		{
			float atten = dotNorLight * attenuation(dist); // light strength from source attenuated for distance
			float spec = specular(posWS.xyz, norWS.xyz, -toLight, cameraPosition.xyz, mat_specularExponent); // specular strength from the light

			target += float4(pl.color, 0.0) * (atten * float4(mat_diffuse, 1.0) + spec * float4(mat_specular, 1.0));
		}
	}

	// Add ambient
	target += float4(mat_ambient * materialColor, 1.0); // defer *col;

	// Add emission
	target += float4(mat_emission, 1.0);

	return target;
}

float4 main(float3 posWS : WSPosition, float3 norWS : WSNormal) : SV_TARGET
{
	return processPixel(mat_diffuse, posWS, norWS);
}

float4 mainTextured(float3 posWS : WSPosition, float3 norWS : WSNormal, float2 tc : Texcoord) : SV_TARGET
{
	return processPixel(tex.Sample(smplr, tc).rgb, posWS, norWS);
}
