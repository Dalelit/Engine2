#include "VSCB0_Scene.hlsli"

#include "VSCB1_Model.hlsli"

float4 main(float3 pos : Position) : SV_POSITION
{
	return mul(mul(float4(pos, 1.0f), modelTransform), cameraTransform);
}

float4 mainInstanced(float3 pos : POSITION, matrix instTransform : InstanceTransform) : SV_POSITION
{
	return mul(mul(float4(pos, 1.0f), instTransform), cameraTransform);
}