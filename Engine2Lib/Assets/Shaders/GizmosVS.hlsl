cbuffer sceneConst : register (b0)
{
	matrix cameraTransform;
}; 

float4 main( float4 pos : POSITION ) : SV_POSITION
{
	return mul(pos, cameraTransform);
}