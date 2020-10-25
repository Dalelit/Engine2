cbuffer sceneConst : register (b0)
{
	matrix cameraTransform;
};

cbuffer modelConst : register (b1)
{
	matrix worldTransform;
};

cbuffer outlineConst : register (b2)
{
	float outlineScale;
};

float4 main(float3 pos : Position) : SV_POSITION
{
	float4 pws = mul(float4(pos * outlineScale, 1.0f), worldTransform);

	return mul( pws, cameraTransform );
}