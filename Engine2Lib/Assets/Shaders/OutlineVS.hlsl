#include "VSCB0_Scene.hlsli"

#include "VSCB1_Model.hlsli"

cbuffer outlineConst : register (b2)
{
	float outlineScale;
};

float4 main(float3 pos : Position) : SV_POSITION
{
	float4 pws = float4(pos * outlineScale, 1.0f); // scale to the outline size
	pws = mul(pws, modelTransform); // transform to the world
	pws = mul(pws, cameraTransform); // transform to the camera

	return pws;
}