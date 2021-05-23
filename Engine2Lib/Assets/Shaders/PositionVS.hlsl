#include "VSCB0_Scene.hlsli"

#include "VSCB1_Model.hlsli"

float4 main(float3 pos : Position) : SV_POSITION
{
	float4 pws = mul(float4(pos, 1.0f), modelTransform);
	return mul(pws, cameraTransform);
}