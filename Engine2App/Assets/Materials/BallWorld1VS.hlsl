cbuffer sceneConst : register (b0)
{
	matrix cameraTransform;
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

	vso.posWS = pos;
	vso.norWS = nor;
	vso.col   = col;
	vso.posSS = mul(float4(pos, 1.0f), cameraTransform);

	//vso.pos = mul(mul(float4(pos, 1.0f), entityTransform), cameraTransform);

	return vso;
}