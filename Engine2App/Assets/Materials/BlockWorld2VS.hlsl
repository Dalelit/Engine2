#include "SceneCBVS.hlsl"

struct VSOut
{
	float3 norWS : WSNormal;
	uint   type  : BlockType;
	float4 posSS : SV_POSITION;
};

VSOut main(float3 pos : Position, float3 nor : Normal, uint type : BlockType)
{
	VSOut vso;

	vso.norWS = nor;
	vso.type = type;
	vso.posSS = mul(float4(pos, 1.0f), cameraTransform);

	return vso;
}