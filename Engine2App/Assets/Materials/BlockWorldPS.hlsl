#include "PixelShaderCommon.hlsl"


float4 main(float3 norWS : WSNormal, uint type : BlockType) : SV_TARGET
{
	float4 col;

	if (type == 1) col = float4(0.8, 0.8, 0.2, 1.0);
	else if (type == 2) col = float4(0.2, 0.8, 0.2, 1.0);
	else if (type == 3) col = float4(0.2, 0.2, 0.8, 1.0);
	else col = float4(1.0, 1.0, 1.0, 1.0);

	norWS = normalize(norWS);

	float3 ligthDirection = float3(1.0, 2.0, 3.0);

	float norDotLight = dot(norWS, normalize(ligthDirection));

	norDotLight = max(0.1, norDotLight);

	return col * norDotLight;
}