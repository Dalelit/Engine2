#include "PixelShaderCommon.hlsl"


cbuffer sceneConst : register (b0)
{
	float4 cameraPosition;
	float4 ambientLight;
	float4 pointLightPosition;
	float4 pointLightColor;
};


float attenuation(float distance)
{
	return 1.0 / (1.0 + 0.045 * distance + 0.0075 * distance * distance);
}

float4 main(float3 posWS : WSPosition, float3 norWS : WSNormal, float4 col : Color) : SV_TARGET
{
		float light = 0.0;
//col = float4(1.0, 1.0, 1.0, 1.0);

norWS = normalize(norWS);

float3 toLight = pointLightPosition.xyz - posWS.xyz;
float norDotLight = dot(norWS, normalize(toLight));

if (norDotLight > 0.0)
{
	const float PI = 3.14159565359;

	float specLight = specular(posWS.xyz, norWS.xyz, pointLightPosition.xyz, cameraPosition.xyz, 5.0);
	light += specLight;

	float att = attenuation(length(toLight)) * norDotLight;
	att /= PI;
	light += att;
}

//float3 toCam = normalize(cameraPosition.xyz - posWS);
//return fresnelSchlick(dot(norWS, toCam), col);

return light * pointLightColor * col + ambientLight * col;
}
