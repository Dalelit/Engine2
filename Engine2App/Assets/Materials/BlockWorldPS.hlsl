#include "PixelShaderCommon.hlsl"


float4 main(float3 norWS : WSNormal) : SV_TARGET
{
	float4 col = float4(0.2, 0.2, 0.8, 1.0);

	norWS = normalize(norWS);

	float3 ligthDirection = float3(1.0, 2.0, 3.0);

	float norDotLight = dot(norWS, normalize(ligthDirection));

	norDotLight = max(0.1, norDotLight);

	return col * norDotLight;
}