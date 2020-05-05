#include "SceneCBVS.hlsl"

struct VSOut
{
	float4 col   : Color;
	float4 posSS : SV_POSITION;
};


VSOut main(float3 pos : POSITION, matrix instTransform : InstanceTransform, float4 instCol : InstanceColor)
{
	VSOut vso;

	vso.posSS = mul(mul(float4(pos, 1.0f), instTransform), cameraTransform);
	vso.col = instCol;

	return vso;
}