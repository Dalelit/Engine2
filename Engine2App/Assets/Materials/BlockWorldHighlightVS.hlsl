#include "..\..\..\Engine2Lib\Assets\Shaders\VSCB0_Scene.hlsli"

cbuffer entityConst : register (b1)
{
	float3 location;
	float  padding;
};

float4 main( float3 pos : POSITION ) : SV_POSITION
{
	return mul(float4(pos + location, 1.0f), cameraTransform);
}