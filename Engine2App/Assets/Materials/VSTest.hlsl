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
	float4 col : Color;
	float4 pos : SV_POSITION;
};

VSOut main(float3 pos : Position, float4 col : Color)
{
	VSOut vso;

	vso.pos = mul(mul(float4(pos, 1.0f), entityTransform), cameraTransform);
	vso.col = col;

	return vso;
}