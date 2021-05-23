#include "VSCB0_Scene.hlsli"

#include "VSCB1_Model.hlsli"

struct VSOut
{
	float3 posWS : WSPosition;
	float3 norWS : WSNormal;
	float4 posSS : SV_POSITION;
};

VSOut main(float3 pos : Position, float3 nor : Normal)
{
	VSOut vso;

	float4 pws = mul(float4(pos, 1.0f), modelTransform);

	vso.posWS = pws.xyz;
	vso.norWS = mul(float4(nor, 1.0f), modelRotation).xyz;
	vso.posSS = mul(pws, cameraTransform);

	return vso;
}