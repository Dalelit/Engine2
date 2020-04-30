cbuffer sceneConst : register (b0)
{
	matrix cameraTransform;
};

cbuffer outlineConst : register (b1)
{
	float outlineScale;
};

float4 main(float3 pos : Position) : SV_POSITION
{
	return mul( float4(pos * outlineScale, 1.0), cameraTransform );
}