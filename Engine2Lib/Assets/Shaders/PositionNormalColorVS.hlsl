cbuffer sceneConst : register (b0)
{
	matrix cameraTransform;
};

cbuffer modelConst : register (b1)
{
	matrix worldTransform;
};

struct VSOut
{
	float3 posWS : WSPosition;
	float3 norWS : WSNormal;
	float4 col   : Color;
	float4 posSS : SV_POSITION;
};

VSOut main(float3 pos : Position, float3 nor : Normal, float4 col : Color)
{
	VSOut vso;

	float4 pws = mul(float4(pos, 1.0f), worldTransform);

	vso.posWS = pws.xyz;
	vso.norWS = nor;
	vso.col = col;
	vso.posSS = mul(pws, cameraTransform);

	return vso;
}