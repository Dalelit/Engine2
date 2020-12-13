#include "VSCB0_Scene.hlsli"

struct VSOut
{
	float4 col   : Color;
	float2 uv    : UV;
	float4 posSS : SV_POSITION;
};


VSOut main(float3 pos : POSITION, float2 uv: UV, matrix instTransform : InstanceTransform, float4 instCol : InstanceColor)
{
	VSOut vso;

	vso.posSS = mul(mul(float4(pos, 1.0f), instTransform), cameraTransform);
	vso.uv = uv;
	vso.col = instCol;

	return vso;
}