cbuffer sceneConst : register (b0)
{
	matrix cameraTransform;
};

cbuffer entityConst : register (b1)
{
	matrix entityTransform;
	matrix entityTransformRotation;
};

float4 main(float3 pos : Position) : SV_POSITION
{
	return mul(mul(float4(pos, 1.0f), entityTransform), cameraTransform);
}
