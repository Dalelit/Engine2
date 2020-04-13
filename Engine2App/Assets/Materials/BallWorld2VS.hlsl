#include "SceneEntityCBVS.hlsl"

struct VSOut
{
	float3 posWS : WSPosition;
	float3 norWS : WSNormal;
	float4 col   : Color;
	float4 posSS : SV_POSITION;
};

VSOut main(float3 pos : Position, float3 nor : Normal, uint  instanceID : SV_InstanceID)
{
	VSOut vso;

	pos.z += cos(instanceID * 3.14 / 5.0);
	pos.x += sin(instanceID * 3.14 / 5.0);
	pos.y += instanceID * 0.5;
	float4 pws = mul(float4(pos, 1.0f), entityTransform);

	vso.posWS = pws.xyz;
	vso.norWS = nor;
	vso.col   = float4(instanceID % 2 * 1.0, 1.0, 1.0, 1.0);
	vso.posSS = mul(pws, cameraTransform);

	return vso;
}