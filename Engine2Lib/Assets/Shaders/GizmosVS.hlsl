cbuffer sceneConst : register (b0)
{
	matrix cameraTransform;
};

struct VSOut
{
	float4 col : Color;
	float4 pos : SV_POSITION;
};

VSOut main(float3 pos : Position, float3 col : Color)
{
	VSOut result;
	result.pos = mul(float4(pos, 1.0), cameraTransform);
	result.col = float4(col, 1.0f);
	return result;
}