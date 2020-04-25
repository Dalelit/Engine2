cbuffer sceneConst : register (b0)
{
	matrix cameraTransform;
}; 

float4 main(float3 pos : Position, matrix instTrans : InstTransform, uint instanceID : SV_InstanceID) : SV_POSITION
{
	return mul(mul(float4(pos, 1.0), instTrans), cameraTransform);
}