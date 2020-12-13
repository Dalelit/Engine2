#include "VSCB0_Scene.hlsli"

#include "VSCB1_Model.hlsli"

float4 main(float3 pos : Position) : SV_POSITION
{
	float4 p = mul(mul(float4(pos, 1.0f), modelTransform), cameraTransform);
	return p;
}