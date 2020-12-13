#include "PS_Shadow.hlsli"
#include "PS_Lighting.hlsli"
#include "PSCB0_Scene.hlsli"
#include "PSCB1_Material.hlsli"

float4 main(float3 posWS : WSPosition, float3 norWS : WSNormal, float4 col : Color) : SV_TARGET
{
	float4 target = 0.0;

	// Change the vertex color to match the diffuse color
	// Generally the model is a white vertex
	col *= float4(mat_diffuse, 1.0);

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
