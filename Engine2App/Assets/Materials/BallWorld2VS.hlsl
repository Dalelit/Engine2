#include "SceneEntityCBVS.hlsl"

struct VSOut
{
	float3 posWS : WSPosition;
	float3 norWS : WSNormal;
	float4 col   : Color;
	float4 posSS : SV_POSITION;
};

VSOut main(float3 pos : Position, float3 nor : Normal, float3 instPos : InstancePos, uint  instanceID : SV_InstanceID)
{
	VSOut vso;

	float4 pws = float4(pos + instPos, 1.0f);

	vso.posWS = pws.xyz;
	vso.norWS = nor;
	vso.col   = float4(instanceID % 2 * 1.0, 1.0, 1.0, 1.0);
	vso.posSS = mul(pws, cameraTransform);

	return vso;
}