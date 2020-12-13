cbuffer sceneConst : register (b0)
{
	matrix cameraTransform;
};

cbuffer modelConst : register (b1)
{
	matrix modelRotation;
	matrix modelTransform;
};

float4 main(float3 pos : Position) : SV_POSITION
{
	float4 p = mul(mul(float4(pos, 1.0f), modelTransform), cameraTransform);
	return p;
}