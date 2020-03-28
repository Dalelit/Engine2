cbuffer sceneConst : register (b0)
{
	matrix cameraTransform;
};

cbuffer entityConst : register (b1)
{
	matrix entityTransform;
	matrix entityTransformRotation;
};

struct VSOut
{
	float3 posWS : worldPosition;
	float4 pos : SV_POSITION;
};

VSOut main(float3 pos : Position)
{
	VSOut vso;

	vso.pos = mul(mul(float4(pos, 1.0f), entityTransform), cameraTransform);
	vso.posWS = (float3)vso.pos;

	return vso;
}