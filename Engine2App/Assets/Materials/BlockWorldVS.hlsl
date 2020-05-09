#include "SceneCBVS.hlsl"

struct VSOut
{
	float3 norWS : WSNormal;
	float4 posSS : SV_POSITION;
};

VSOut main(float3 pos : Position, float3 nor : Normal, float3 instLocation : InstanceLocation)
{
	VSOut vso;

	vso.posSS = mul(float4(pos + instLocation, 1.0f), cameraTransform);
	vso.norWS = nor;

	return vso;
}