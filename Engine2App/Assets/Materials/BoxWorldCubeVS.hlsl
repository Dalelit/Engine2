#include "SceneEntityCBVS.hlsl"

struct VSOut
{
	float3 posMS : modelPosition;
	float4 pos   : SV_POSITION;
};

VSOut main(float3 pos : Position)
{
	VSOut vso;
	vso.posMS = pos;

	float4 posWS = mul(float4(pos, 1.0f), entityTransform);
	vso.pos = mul(posWS, cameraTransform);

	return vso;
}
