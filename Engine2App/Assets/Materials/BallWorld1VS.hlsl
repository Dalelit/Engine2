#include "SceneEntityCBVS.hlsl"

struct VSOut
{
	float4 posWS : WSPosition;
	float3 norWS : WSNormal;
	float4 col   : Color;
	float4 posSS : SV_POSITION;
};

VSOut main(float3 pos : Position, float3 nor : Normal, float4 col : Color)
{
	VSOut vso;

	vso.posWS = mul(float4(pos, 1.0f), entityTransform);
	vso.norWS = nor;
	vso.col   = col;
	vso.posSS = mul(vso.posWS, cameraTransform);

	return vso;
}