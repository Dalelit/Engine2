#include "Common.hlsli"
#include "PSCB0_Scene.hlsli"
#include "PS_Shadow.hlsli"

cbuffer materialConst : register (b1)
{
	float3 mat_color;
};


float4 main(float3 posWS : WSPosition, float3 norWS : WSNormal) : SV_TARGET
{
	return float4(mat_color, 1.0);
}
