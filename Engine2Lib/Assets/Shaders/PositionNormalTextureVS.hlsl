cbuffer sceneConst : register (b0)
{
	matrix cameraTransform;
};

cbuffer modelConst : register (b1)
{
	matrix modelRotation;
	matrix modelTransform;
};

struct VSOut
{
	float3 posWS : WSPosition;
	float3 norWS : WSNormal;
	float2 tex   : Texcoord;
	float4 posSS : SV_POSITION;
};

VSOut main(float3 pos : Position, float3 nor : Normal, float2 tex : Texcoord)
{
	VSOut vso;

	float4 pws = mul(float4(pos, 1.0f), modelTransform);

	vso.posWS = pws.xyz;
	vso.norWS = mul(float4(nor, 1.0f), modelRotation).xyz;
	vso.tex = tex;
	vso.posSS = mul(pws, cameraTransform);

	return vso;
}